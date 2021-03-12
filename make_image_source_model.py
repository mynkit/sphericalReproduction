import numpy as np
from scipy.io import wavfile
from scipy.signal import fftconvolve
import pyroomacoustics as pra
import matplotlib.pyplot as plt
import json
import csv

room_setting_path = './bin/data/room_setting.json'
image_source_model_path = './bin/data/image_source_model.csv'


def read_room_setting(setting_file: str) -> dict:
    with open(setting_file) as f:
        room_setting = json.load(f)
    return room_setting


def setup_room(corners: np.ndarray, height: int, microphone: [float], sound_source: [float], max_order: int) -> pra.room.Room:
    xmax = max([xy[0] for xy in corners.T])
    xmin = min([xy[0] for xy in corners.T])
    ymax = max([xy[1] for xy in corners.T])
    ymin = min([xy[1] for xy in corners.T])
    zmax = height
    zmin = 0
    sound_source = sound_source
    mic = microphone
    room = pra.Room.from_corners(corners, max_order=max_order)
    room.extrude(height)
    R = np.array([[mic[0]], [mic[1]], [mic[2]]])
    room.add_microphone(R)
    try:
        room.add_source(sound_source)
    except ValueError:
        show_room(room)
        raise ValueError('The source must be added inside the room')
    if not room.is_inside(mic):
        show_room(room)
        raise ValueError('The microphone must be added inside the room')
    room.image_source_model()
    return room


def show_room(room: pra.room.Room):
    _, ax = room.plot(img_order=0)

    [xmin, ymin, zmin] = np.min([np.min(wall.corners.T, axis=0)
                                 for wall in room.walls], axis=0)
    [xmax, ymax, zmax] = np.max([np.max(wall.corners.T, axis=0)
                                 for wall in room.walls], axis=0)

    [xmin, ymin, zmin] = np.min(
        np.array([[xmin, ymin, zmin], room.mic_array.R.flatten()]).T, axis=1)
    [xmax, ymax, zmax] = np.max(
        np.array([[xmax, ymax, zmax], room.mic_array.R.flatten()]).T, axis=1)

    if room.sources:
        [xmin, ymin, zmin] = np.min(
            np.array([[xmin, ymin, zmin], room.sources[0].position]).T, axis=1)
        [xmax, ymax, zmax] = np.max(
            np.array([[xmax, ymax, zmax], room.sources[0].position]).T, axis=1)

    ax.set_xlim([xmin, xmax])
    ax.set_ylim([ymin, ymax])
    ax.set_zlim([zmin, zmax])
    ax.set(xlabel='X', ylabel='Y')
    ax.annotate('x: microphone ●: soundsource', (0, 0.08))
    plt.show()


def get_distances_orders(room: pra.room.Room):
    distances = []
    for x, y, z in zip(room.sources[0].images[0], room.sources[0].images[1], room.sources[0].images[2]):
        distances.append(np.linalg.norm(
            [x, y, z] - room.mic_array.R.flatten()))
    orders = room.sources[0].orders
    return distances, orders


def adjust_distances(distances: [float], orders: [int]):
    """最寄り距離を0となるよう補正し、最寄り点は削除する

    この補正がないと1音目は遅れて聴こえるようになってしまうための補正.
    
    """
    distances_adjusted = [distance - min(distances) for distance in distances]
    distances_adjusted = distances_adjusted[1:]
    orders_adjusted = orders[1:]
    return distances_adjusted, orders_adjusted


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('-p', '--preview', action='store_true')

    args = parser.parse_args()

    room_setting = read_room_setting(room_setting_path)
    corners = np.array(room_setting['corners']).T
    height = room_setting['height']
    microphone = room_setting['microphone']
    sound_source = room_setting['sound_source']
    max_order = room_setting['max_order']
    room = setup_room(corners, height, microphone, sound_source, max_order)

    distances, orders = get_distances_orders(room)
    distances, orders = adjust_distances(distances, orders)
    
    if not args.preview:
        with open(image_source_model_path, 'w') as f:
            writer = csv.writer(f)
            for distance, order in zip(distances, orders):
                writer.writerow([distance, order])
        print(
            f'ImageSource Setup Completed! Look at {image_source_model_path}')
    else:
        show_room(room)
        sorted_distances = sorted(distances)
        plt.title('Show Distances Transition')
        plt.xlabel('index')
        plt.ylabel('distance')
        plt.plot(sorted(distances))
        plt.show()


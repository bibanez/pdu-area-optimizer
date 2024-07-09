import pandas as pd
import matplotlib.pyplot as plt
import imageio.v2 as imageio
import os

def create_image_from_csv(csv_file, image_file):
    df = pd.read_csv(csv_file, header=None)
    plt.imshow(df, cmap='viridis', interpolation='nearest')
    plt.colorbar()
    plt.title(f"Iteration {os.path.splitext(os.path.basename(csv_file))[0].split('_')[-1]}")
    plt.savefig(image_file)
    plt.close()

def create_gif(image_folder, gif_file):
    image_files = sorted([os.path.join(image_folder, f) for f in os.listdir(image_folder) if f.endswith('.png')])
    
    with imageio.get_writer(gif_file, mode='I', duration=0.5) as writer:
        for image_file in image_files:
            image = imageio.imread(image_file)
            writer.append_data(image)

def main():
    csv_folder = 'csv_files'  # Folder where CSV files are saved by C++ program
    image_folder = 'images'   # Folder to save images temporarily
    gif_file = 'animation.gif'

    # Ensure image folder exists or create it
    if not os.path.exists(image_folder):
        os.makedirs(image_folder)

    # Convert CSV files to images
    csv_files = sorted([os.path.join(csv_folder, f) for f in os.listdir(csv_folder) if f.startswith('output_') and f.endswith('.csv')])

    for csv_file in csv_files:
        image_file = os.path.join(image_folder, f"{os.path.splitext(os.path.basename(csv_file))[0]}.png")
        create_image_from_csv(csv_file, image_file)

    # Create animated gif from images
    create_gif(image_folder, gif_file)

    # Clean up image files
    #for image_file in os.listdir(image_folder):
    #    os.remove(os.path.join(image_folder, image_file))
    #os.rmdir(image_folder)

if __name__ == "__main__":
    main()

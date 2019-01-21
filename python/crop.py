from PIL import Image
import argparse

Image.MAX_IMAGE_PIXELS = None

def crop(input_filename, coords, output_filename):
    """
    @param image_path: The path to the image to edit
    @param coords: A tuple of x/y coordinates (x1, y1, x2, y2)
    @param saved_location: Path to save the cropped image
    """
    image = Image.open(input_filename)
    cropped_image = image.crop(coords)
    cropped_image.save(output_filename)
    #cropped_image.show()
 
 
if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument("filename", help="Input filename")
	args = parser.parse_args()
	
    crop(args.filename, (0, 0, 11408, 10661), 'output.png')
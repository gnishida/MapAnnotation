from PIL import Image
import argparse
import random
import os

Image.MAX_IMAGE_PIXELS = None

def generate_data(image_filename, annotation_filename, num_traindata, num_valdata, num_testdata):
	image = Image.open(image_filename).convert('RGB')
	annotation = Image.open(annotation_filename).convert('RGB')
	width, height = image.size
	
	pixels = annotation.load()
	for y in range(height):
		for x in range(width):
			if pixels[x, y] == (255, 255, 255):
				pixels[x, y] = (0, 0, 255)
			else:
				pixels[x, y] = (255, 0, 0)
	
	dict = {
		"train": num_traindata,
		"val": num_valdata,
		"test": num_testdata
	}
	print(dict)
	
	for type in dict:
		if not os.path.exists(type):
			os.mkdir(type)
		
		
		for i in range(dict[type]):
			x = random.randint(0, width - 256)
			y = random.randint(0, height - 256)
			cropped_image = image.crop((x, y, x + 256, y + 256))
			cropped_annotation = annotation.crop((x, y, x + 256, y + 256))
			output_image = Image.new('RGB', (512, 256))
			output_image.paste(cropped_image, (0, 0))
			output_image.paste(cropped_annotation, (256, 0))
			output_image.save(type + "/" + str(i + 1) + ".png")			
		
if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument("image", help="Image filename")
	parser.add_argument("annotation", help="Annotation filename")
	parser.add_argument("num_traindata", type=int, help="#train data")
	parser.add_argument("num_valdata", type=int, help="#val data")
	parser.add_argument("num_testdata", type=int, help="#test data")
	args = parser.parse_args()
	
	generate_data(args.image, args.annotation, args.num_traindata, args.num_valdata, args.num_testdata)
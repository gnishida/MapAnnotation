from PIL import Image
import urllib.parse
import urllib.request
import io
import time

def getSatelliteImage(lat, lon, zoom):
	url = "https://maps.googleapis.com/maps/api/staticmap?center=" + str(lat) + "," + str(lon) + "&zoom=" + str(zoom) + "&size=640x640&maptype=satellite&key="
	print("Request: " + url)
	f=urllib.request.urlopen(url)
	im=Image.open(io.BytesIO(f.read()))
	#im = Image.frombytes("L", (640, 640), f.read())
	im = im.crop((0, 0, 640, 620))
	im = im.convert("RGB")
	#filename = "result_" + str(lat) + "_" + str(lon) + "_" + str(zoom) + ".png"
	#print("--> saved to " + filename)
	#im.save(filename)
	return im

def findPositionToStitchHorizontal(prev_im, cur_im):
	bestCost = 1e10
	for dx in range(100):
		cost = 0
		for y in range(prev_im.size[1]):
			r, g, b = prev_im.getpixel((prev_im.size[0] - 1 - dx, y))
			r2, g2, b2 = cur_im.getpixel((0, y))
			cost += (r - r2) ** 2 + (g - g2) ** 2 + (b - b2) ** 2

		if cost < bestCost:
			bestCost = cost
			best_x = prev_im.size[0] - 1 - dx
	
	return best_x

def findPositionToStitchVertical(prev_im, cur_im):
	bestCost = 1e10
	for dy in range(100):
		cost = 0
		for x in range(prev_im.size[0]):
			r, g, b = prev_im.getpixel((x, prev_im.size[1] - 1 - dy))
			r2, g2, b2 = cur_im.getpixel((x, 0))
			cost += (r - r2) ** 2 + (g - g2) ** 2 + (b - b2) ** 2
			
		if cost < bestCost:
			bestCost = cost
			best_y = prev_im.size[1] - 1 - dy
	
	return best_y
	
def getLargeSatelliteImage(topleft_lat, topleft_lon, zoom, numRows, numCols):
	lat = topleft_lat
	lon_step = 0.00085
	lat_step = 0.00063
	final_image = Image.new("RGB", (640 * numCols, 620 * numRows))
	prev_x_offset = []
	x_offset = []
	
	y = 0
	for row in range(numRows):
		x = 0
		prev_x_offset = []
		x_offset = []
		lon = topleft_lon
		prev_im_vald = False
		
		for col in range(numCols):
			try:
				cur_im = getSatelliteImage(lat, lon, zoom)		

				if col == 0:
					if row > 0:
						dy = findPositionToStitchVertical(prev_row_im, cur_im)
						y += dy
					prev_row_im = cur_im
				elif  not prev_im_vald:
					x = prev_x_offset[col]
				else:
					dx = findPositionToStitchHorizontal(prev_im, cur_im)
					#print("position to stitch: " + str(dx))
					x += dx

				x_offset.append(x)
					
				final_image.paste(cur_im, (x, y))			
				prev_im = cur_im
				prev_im_vald = True
			except:
				prev_im_vald = False
			
			lon += lon_step
				
			time.sleep(2)
		
		prev_x_offset = x_offset
		lat -= lat_step
		
	return final_image
	
#result = getLargeSatelliteImage(37.6977, -122.483, 20, 2, 3)
result = getLargeSatelliteImage(37.6980, -122.4944, 20, 18, 18)
result.save("result.png")
#include "Canvas.h"
#include <QPainter>
#include <iostream>
#include <QFileInfoList>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QResizeEvent>
#include <QDomDocument>
#include <unordered_map>

Canvas::Canvas(QWidget *parent) : QWidget(parent) {
	ctrlPressed = false;
	shiftPressed = false;

	min_image_scale = 1.0f;
	image_scale = 1.0f;

	adding_new_sidewalk = false;
}

void Canvas::loadImage(const QString& filename) {
	image_filename = filename;
	image_1 = QImage(filename).convertToFormat(QImage::Format_ARGB32);
	createMipmap(image_1);
	
	image_scale = std::min((float)width() / image_1.width(), (float)height() / image_1.height());
	min_image_scale = image_scale;
	image = image_1.scaled(image_1.width() * image_scale, image_1.height() * image_scale);

	image_pos = glm::vec2(0, 0);

	update();
}

void Canvas::saveImage(const QString& filename) {
	QImage image = QImage(image_1.width(), image_1.height(), QImage::Format_RGB888);
	QPainter painter(&image);

	painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0, 255));

	// draw sidewalks
	painter.setPen(QPen(QColor(255, 255, 255, 255)));
	painter.setBrush(QBrush(QColor(255, 255, 255, 255)));
	for (const auto& sidewalk : sidewalks) {
		QPolygon polygon;
		for (const auto& p : sidewalk) {
			polygon.push_back(QPoint(p.x, p.y));
		}
		painter.drawPolygon(polygon);
	}

	image.save(filename);
}

void Canvas::loadXML(const QString& filename) {
	QDomDocument doc;
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file)) return;

	sidewalks.clear();

	QDomElement root = doc.documentElement();
	QDomElement shape_node = root.firstChild().toElement();
	while (!shape_node.isNull()) {
		if (shape_node.tagName() == "image") {
			QString filename = shape_node.attribute("filename");
			loadImage(filename);
		}
		else if (shape_node.tagName() == "osm") {
			QString filename = shape_node.attribute("filename");
			loadOSM(filename);

			osm_offset.x = shape_node.attribute("osm_offset_x").toFloat();
			osm_offset.y = shape_node.attribute("osm_offset_y").toFloat();
			osm_scale.x = shape_node.attribute("osm_scale_x").toFloat();
			osm_scale.y = shape_node.attribute("osm_scale_y").toFloat();
		}
		else if (shape_node.tagName() == "sidewalk") {
			std::vector<glm::vec2> sidewalk;

			QDomElement node = shape_node.firstChild().toElement();
			while (!node.isNull()) {
				float x = node.attribute("x").toFloat();
				float y = node.attribute("y").toFloat();
				sidewalk.emplace_back(x, y);
				node = node.nextSiblingElement();
			}

			sidewalks.push_back(sidewalk);
		}

		shape_node = shape_node.nextSiblingElement();
	}

	update();
}

void Canvas::loadOSM(const QString& filename) {
	QDomDocument doc;
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file)) return;

	osm_filename = filename;
	
	std::unordered_map<unsigned long long, glm::vec2> nodes;
	roads.clear();
	buildings.clear();

	QDomElement root = doc.documentElement();
	QDomElement element = root.firstChild().toElement();
	while (!element.isNull()) {
		if (element.tagName() == "node") {
			unsigned long long id = element.attribute("id").toULongLong();
			double lon = element.attribute("lon").toDouble();
			double lat = element.attribute("lat").toDouble();
			nodes[id] = { lon, lat };
		}
		else if (element.tagName() == "way") {
			if (!element.hasAttribute("visible") || element.attribute("visible") == "true") {
				std::vector<glm::vec2> polygon;
				QString type;

				QDomElement child_element = element.firstChild().toElement();
				while (!child_element.isNull()) {
					if (child_element.tagName() == "nd") {
						unsigned long long id = child_element.attribute("ref").toULongLong();
						polygon.push_back(nodes[id]);
					}
					else if (child_element.tagName() == "tag") {
						QString k = child_element.attribute("k");
						if (k == "building") type = "building";
						else if (k == "highway") type = "highway";
					}

					child_element = child_element.nextSiblingElement();
				}

				if (type == "building") {
					buildings.push_back(polygon);
				}
				else if (type == "highway") {
					roads.push_back(polygon);
				}
			}
		}

		element = element.nextSiblingElement();
	}

	osm_offset = glm::vec2(0, 0);
	osm_scale = glm::vec2(image_1.width() / 0.0151, image_1.height() / 0.0118);

	update();
}

void Canvas::saveXML(const QString& filename) {
	QFile file(filename);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		std::cerr << "Error saving XML file.";
		file.close();
		return;
	}

	QDomDocument xml;

	QDomElement root = xml.createElement("map");
	xml.appendChild(root);

	// image node
	if (!image_filename.isEmpty()) {
		QDomElement image_node = xml.createElement("image");
		image_node.setAttribute("filename", image_filename);
		root.appendChild(image_node);
	}

	// osm node
	if (!osm_filename.isEmpty()) {
		QDomElement osm_node = xml.createElement("osm");
		osm_node.setAttribute("filename", osm_filename);
		osm_node.setAttribute("osm_offset_x", osm_offset.x);
		osm_node.setAttribute("osm_offset_y", osm_offset.y);
		osm_node.setAttribute("osm_scale_x", osm_scale.x);
		osm_node.setAttribute("osm_scale_y", osm_scale.y);
		root.appendChild(osm_node);
	}

	for (const auto& sidewalk : sidewalks) {
		// sidewalk node
		QDomElement sidwalk_node = xml.createElement("sidewalk");

		for (const auto& p : sidewalk) {
			QDomElement node = xml.createElement("node");
			node.setAttribute("x", p.x);
			node.setAttribute("y", p.y);
			sidwalk_node.appendChild(node);
		}

		root.appendChild(sidwalk_node);
	}

	QTextStream output(&file);
	output << xml.toString();

	file.close();
}

void Canvas::createMipmap(const QImage& image) {
	image_15 = image.scaled(image_1.width() * 1.5, image.height() * 1.5);
	image_125 = image.scaled(image_1.width() * 1.25, image.height() * 1.25);
	image_08 = image.scaled(image_1.width() * 0.8, image.height() * 0.8);
	image_064 = image.scaled(image_1.width() * 0.64, image.height() * 0.64);
	image_0512 = image.scaled(image_1.width() * 0.512, image.height() * 0.512);
	image_04096 = image.scaled(image_1.width() * 0.4096, image.height() * 0.4096);
	image_032768 = image.scaled(image_1.width() * 0.32768, image.height() * 0.32768);
}

void Canvas::updateImage(float& image_scale) {
	if (image_scale > 1.35) {
		image_scale = 1.5;
		image = image_15;
	}
	else if (image_scale > 1.1) {
		image_scale = 1.25f;
		image = image_125;
	}
	else if (image_scale > 0.9) {
		image_scale = 1.0f;
		image = image_1;
	}
	else if (image_scale > 0.7) {
		image_scale = 0.8f;
		image = image_08;
	}
	else if (image_scale > 0.6) {
		image_scale = 0.64;
		image = image_064;
	}
	else if (image_scale > 0.45) {
		image_scale = 0.512;
		image = image_0512;
	}
	else if (image_scale > 0.36) {
		image_scale = 0.4096;
		image = image_04096;
	}
	else if (image_scale > 0.29) {
		image_scale = 0.32768;
		image = image_032768;
	}
	else {
		image = image_1.scaled(image_1.width() * image_scale, image_1.height() * image_scale);
	}
}

void Canvas::keyPressEvent(QKeyEvent* e) {
	ctrlPressed = false;
	shiftPressed = false;

	if (e->modifiers() & Qt::ControlModifier) {
		ctrlPressed = true;
	}
	if (e->modifiers() & Qt::ShiftModifier) {
		shiftPressed = true;
	}

	switch (e->key()) {
	case Qt::Key_Right:
		if (ctrlPressed) {
			osm_scale.x += image_1.width() * 0.1;
		}
		else {
			osm_offset.x++;
		}
		break;
	case Qt::Key_Left:
		if (ctrlPressed) {
			osm_scale.x -= image_1.width() * 0.1;
		}
		else {
			osm_offset.x--;
		}
		break;
	case Qt::Key_Up:
		if (ctrlPressed) {
			osm_scale.y -= image_1.height() * 0.1;
		}
		else {
			osm_offset.y--;
		}
		break;
	case Qt::Key_Down:
		if (ctrlPressed) {
			osm_scale.y += image_1.height() * 0.1;
		}
		else {
			osm_offset.y++;
		}
		break;
	case Qt::Key_Escape:
		adding_new_sidewalk = false;
		setMouseTracking(false);
		new_sidewalk.clear();
		break;
	}

	//std::cout << "scale=(" << osm_scale.x << "," << osm_scale.y << "), offset=(" << osm_offset.x << "," << osm_offset.y << ")" << std::endl;

	update();
}

void Canvas::keyReleaseEvent(QKeyEvent* e) {
	switch (e->key()) {
	case Qt::Key_Control:
		ctrlPressed = false;
		break;
	case Qt::Key_Shift:
		shiftPressed = false;
		break;
	default:
		break;
	}
}

void Canvas::paintEvent(QPaintEvent* e) {
	QPainter painter(this);

	painter.fillRect(0, 0, width(), height(), QColor(128, 128, 128, 255));
	if (!image.isNull()) {
		painter.drawImage(image_pos.x, image_pos.y, image);
	}

	// draw roads
	painter.setPen(QPen(QColor(255, 255, 0, 128), 3));
	painter.setBrush(QBrush(QColor(255, 255, 0, 128)));
	for (const auto& road : roads) {
		QPolygon polygon;
		for (const auto& p : road) {
			polygon.push_back(QPoint(((p.x + 122.4945) * osm_scale.x + osm_offset.x) * image_scale + image_pos.x, ((37.6986 - p.y) * osm_scale.y + osm_offset.y) * image_scale + image_pos.y));
		}
		painter.drawPolyline(polygon);
	}

	// draw buildings
	painter.setPen(QPen(QColor(0, 255, 0, 128), 3));
	painter.setBrush(QBrush(QColor(0, 255, 0, 128)));
	for (const auto& building : buildings) {
		QPolygon polygon;
		for (const auto& p : building) {
			polygon.push_back(QPoint(((p.x + 122.4945) * osm_scale.x + osm_offset.x) * image_scale + image_pos.x, ((37.6986 - p.y) * osm_scale.y + osm_offset.y) * image_scale + image_pos.y));
		}
		painter.drawPolygon(polygon);
	}

	// draw sidewalks
	painter.setPen(QPen(QColor(0, 0, 255, 128)));
	painter.setBrush(QBrush(QColor(0, 0, 255, 128)));
	for (const auto& sidewalk : sidewalks) {
		QPolygon polygon;
		for (const auto& p : sidewalk) {
			polygon.push_back(QPoint(p.x * image_scale + image_pos.x, p.y * image_scale + image_pos.y));
		}
		painter.drawPolygon(polygon);
	}

	// draw new sidewalk that is being drawn
	if (adding_new_sidewalk) {
		painter.setPen(QPen(QColor(0, 0, 255, 128), 2));
	
		QPolygon polygon;
		for (const auto& p : new_sidewalk) {
			polygon.push_back(QPoint(p.x * image_scale + image_pos.x, p.y * image_scale + image_pos.y));
		}
		painter.drawPolygon(polygon);
	}
}

void Canvas::mousePressEvent(QMouseEvent* e) {
	prev_pt = QPoint(e->pos());
	if (adding_new_sidewalk) {
		new_sidewalk.back() = glm::vec2((e->x() - image_pos.x) / image_scale, (e->y() - image_pos.y) / image_scale);
		new_sidewalk.emplace_back((e->x() - image_pos.x) / image_scale, (e->y() - image_pos.y) / image_scale);
	}
	else if (e->button() == Qt::LeftButton) {
		adding_new_sidewalk = true;
		setMouseTracking(true);

		// add sidewalk polygon point
		new_sidewalk.emplace_back((e->x() - image_pos.x) / image_scale, (e->y() - image_pos.y) / image_scale);
		new_sidewalk.emplace_back((e->x() - image_pos.x) / image_scale, (e->y() - image_pos.y) / image_scale);
	}
	update();
}

void Canvas::mouseMoveEvent(QMouseEvent* e) {
	if (adding_new_sidewalk) {
		// add sidewalk polygon point
		new_sidewalk.back() = glm::vec2((e->x() - image_pos.x) / image_scale, (e->y() - image_pos.y) / image_scale);
	}
	else if (e->buttons() & Qt::RightButton) {
		// move the image
		image_pos.x += (e->x() - prev_pt.x());
		image_pos.y += (e->y() - prev_pt.y());
	}

	prev_pt = QPoint(e->pos());
	update();
}

void Canvas::mouseDoubleClickEvent(QMouseEvent* e) {
	if (adding_new_sidewalk) {
		adding_new_sidewalk = false;
		new_sidewalk.emplace_back((e->x() - image_pos.x) / image_scale, (e->y() - image_pos.y) / image_scale);
		sidewalks.push_back(new_sidewalk);
		new_sidewalk.clear();

		setMouseTracking(false);
		update();
	}
}

void Canvas::mouseReleaseEvent(QMouseEvent* e) {
}

void Canvas::wheelEvent(QWheelEvent* e) {
	float old_image_scale = image_scale;
	image_scale *= (e->delta() > 0 ? 1.2 : 0.8);
	image_scale = std::min(std::max(min_image_scale, image_scale), 1.25f);
	updateImage(image_scale);

	image_pos.x = width() * 0.5 - (width() * 0.5 - image_pos.x) * image_scale / old_image_scale;
	image_pos.y = height() * 0.5 - (height() * 0.5 - image_pos.y) * image_scale / old_image_scale;
	
	update();
}

void Canvas::resizeEvent(QResizeEvent* e) {
	if (!image_1.isNull()) {
		image_scale = std::min((float)width() / image_1.width(), (float)height() / image_1.height());
		updateImage(image_scale);
	}
}


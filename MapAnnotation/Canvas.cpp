#include "Canvas.h"
#include <QPainter>
#include <iostream>
#include <QFileInfoList>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QResizeEvent>

Canvas::Canvas(QWidget *parent) : QWidget(parent) {
	ctrlPressed = false;
	shiftPressed = false;

	min_image_scale = 1.0f;
	image_scale = 1.0f;

	adding_new_sidewalk = false;
}

void Canvas::loadImage(const QString& filename) {
	image_1 = QImage(filename).convertToFormat(QImage::Format_ARGB32);
	image_08 = image_1.scaled(image_1.width() * 0.8, image_1.height() * 0.8);
	image_064 = image_1.scaled(image_1.width() * 0.64, image_1.height() * 0.64);
	image_0512 = image_1.scaled(image_1.width() * 0.512, image_1.height() * 0.512);
	image_04096 = image_1.scaled(image_1.width() * 0.4096, image_1.height() * 0.4096);
	image_032768 = image_1.scaled(image_1.width() * 0.32768, image_1.height() * 0.32768);

	image_scale = std::min((float)width() / image_1.width(), (float)height() / image_1.height());
	min_image_scale = image_scale;
	image = image_1.scaled(image_1.width() * image_scale, image_1.height() * image_scale);

	image_pos = glm::vec2(0, 0);

	update();
}

void Canvas::saveImage(const QString& filename) {
	grab().save(filename);
}

void Canvas::updateImage(float& image_scale) {
	if (image_scale > 0.9) {
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
	case Qt::Key_Escape:
		setMouseTracking(false);
		new_sidewalk.clear();
		break;
	}

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
	image_scale = std::min(std::max(min_image_scale, image_scale), 1.0f);
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


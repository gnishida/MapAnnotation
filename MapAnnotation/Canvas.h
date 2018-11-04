#ifndef CANVAS_H
#define CANVAS_H

#include <vector>
#include <QWidget>
#include <QKeyEvent>
#include <glm/glm.hpp>

class Canvas : public QWidget {
private:
	// image mipmaps
	QImage image_1;
	QImage image_08;
	QImage image_064;
	QImage image_0512;
	QImage image_04096;
	QImage image_032768;

	QImage image;
	QPoint prev_pt;
	float image_scale;
	float min_image_scale;
	glm::vec2 image_pos;

	std::vector<std::vector<glm::vec2>> sidewalks;
	std::vector<glm::vec2> new_sidewalk;
	bool adding_new_sidewalk;

	bool ctrlPressed;
	bool shiftPressed;
	
public:
	Canvas(QWidget *parent = NULL);

	void loadImage(const QString& filename);
	void saveImage(const QString& filename);
	void updateImage(float& image_scale);
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

protected:
	void paintEvent(QPaintEvent* e);
	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void mouseDoubleClickEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
	void wheelEvent(QWheelEvent* e);
	void resizeEvent(QResizeEvent* e);
};

#endif // CANVAS_H

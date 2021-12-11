
// Figure texture is used to draw figure as background of a figure that can stretch or repeat.
// But you can not create brushes using figure texture.
// Be cautious about recursion.
class FigureTexture : public Texture {
private:
	const Figure& figure;
public:
	FigureTexture(const Figure& figure, Size size) : Texture(size), figure(figure) {}
	Ref<Brush*> GetBrush(Tile& tile) const override { return nullptr; }
	void DrawOn(Tile& tile, Rect region) const override {

	}
};




class GridLinePiece : public Figure {
private:
	Color line_color;
	uint line_width;
	Color background_color;
public:
	GridLinePiece(Color line_color, uint line_width, Color background_color = color_transparent) :
		line_color(line_color), line_width(line_width), background_color(background_color) {
	}
	// The size of the region is the size of a grid, only need to draw the left and top edge.
	void DrawOn(Tile& tile, Rect region) const override {
		if (background_color != color_transparent) {
			ColorFigure color(background_color);
			color.DrawOn(tile, region);
		}
		Line left_line(point_zero, Point(0, static_cast<int>(region.size.height)), line_color, line_width);
		left_line.DrawOn(tile, region);
		Line top_line(point_zero, Point(static_cast<int>(region.size.width), 0), line_color, line_width);
		top_line.DrawOn(tile, region);
	}
};




// The Texture abstract base class.
// Texture is used for drawing background, which might be color, bitmap or figures.
// Texture can be converted to brushes to fill figures, and can be drawed directly on tiles.
// The DrawOn() virtual function draws a texture on the region of a layer.
// Texture keeps its size and initial position, and whether it will repeat or
//   stretch when drawed on a region.

struct Brush {};  // The brush alias to Direct2D brushes.

class Texture {
public:
	virtual Ref<Brush*> GetBrush(Tile& tile) const pure;
	virtual void DrawOn(Tile& tile, Rect region) const pure;
};


class ColorTexture : public Texture {
private:
	Color color;
public:
	ColorTexture(Color color) : Texture(), color(color) {}
	Ref<Brush*> GetBrush(Tile& tile) const override;
	void DrawOn(Tile& tile, Rect region) const override;
};

enum class FillingStyle : ushort { None, Repeat, Stretch };


class ImageTexture : public Texture {
private:
	const Image& image;
	Point start_position;
	FillingStyle x_filling, y_filling;

public:
	ImageTexture(const Image& image,
				 Point start_position = point_zero,
				 FillingStyle x_filling = FillingStyle::Repeat,
				 FillingStyle y_filling = FillingStyle::Repeat) :
		image(image), start_position(start_position), x_filling(x_filling), y_filling(y_filling) {
	}
	Ref<Brush*> GetBrush(Tile& tile) const override;
	void DrawOn(Tile& tile, Rect region) const override;
};



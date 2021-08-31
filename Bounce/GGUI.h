#pragma once
#include <memory>
#include <vector>
#include <functional>
#include "Vector2d.h"
#include <iostream>

enum class MouseButtonGG : int 
{
	LEFT = 0,
	RIGHT = 1,
	MIDDLE = 2
};

enum class RenderStyleGG : int 
{
	ROOT = 0,
	TEXT = 1,
	TEXT_BORDERS = 2,
	BUTTON = 3,
};

struct Element 
{
	// Relative position to parent
	Vector2 position; // top-left coordinate
	Vector2 size;	  // (horizontal, vertical) size of box
	std::string text;
	RenderStyleGG render_style;

	//bool is_hovered = false; // TODO
	bool is_focus = false;

	std::shared_ptr<Element> parent = nullptr;
	std::vector<std::shared_ptr<Element>> children;

	Element(Vector2 position, Vector2 size, std::string text) : Element(position, size, text, RenderStyleGG::TEXT) {}
	Element(Vector2 position, Vector2 size, std::string text, RenderStyleGG style)
	{
		this->position = position;
		this->size = size;
		this->text = text;
		this->render_style = style;
	}

	bool contains_pos(Vector2 pos)
	{
		Vector2 cumulative = position;
		
		auto parent = this->parent;
		while (parent != nullptr) {
			cumulative = cumulative + parent->position;
			parent = parent->parent;
		}

		return pos.x >= cumulative.x
			&& pos.y >= cumulative.y
			&& pos.x < cumulative.x + size.x
			&& pos.y < cumulative.y + size.y;
	}

	virtual void on_click(MouseButtonGG button, Vector2 position) 
	{
		std::cout << "[Element] on_click" << std::endl;
	};
};

using ButtonCallback = std::function<void(MouseButtonGG button, Vector2 position)>;

struct Button : Element {
	ButtonCallback func;

	Button(Vector2 position, Vector2 size, std::string text, ButtonCallback func)
		: Element(position, size, text, RenderStyleGG::BUTTON)
	{
		this->func = func;
	}

	virtual void on_click(MouseButtonGG button, Vector2 position) override 
	{
		func(button, position);
	};
};

class GGUI
{
public:
	std::shared_ptr<Element> root;
	std::shared_ptr<Element> focus;
	
	GGUI(Vector2 root_size);

	void register_click(MouseButtonGG button, Vector2 position);

	void push_child_element(std::shared_ptr<Element> parent, std::shared_ptr<Element> element);
};


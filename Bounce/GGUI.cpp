#include "GGUI.h"

GGUI::GGUI(Vector2 root_size)
{
	Vector2 position = { 0, 0 };
	root = std::make_shared<Element>(position, root_size, std::string(), RenderStyleGG::ROOT);
	focus = root;
}

void GGUI::register_click(MouseButtonGG button, Vector2 position)
{
	// switch focus to element under mouse recursively to lowest element;
	std::shared_ptr<Element> root = focus->contains_pos(position) ? focus : this->root;

	bool found_child;
	do {
		found_child = false;
		for (auto child: root->children) 
		{
			if (child->contains_pos(position)) 
			{
				root = child;
				found_child = true;
				break;
			}
		}
	} while (found_child);

	if (focus != root)
	{
		focus->is_focus = false;
		focus = root;
		root->is_focus = true;
		root->on_click(button, position);

		std::cout << "change focus" << std::endl;
	}
}

void GGUI::push_child_element(std::shared_ptr<Element> parent, std::shared_ptr<Element> element)
{
	parent->children.push_back(element);
	element->parent = parent;
}

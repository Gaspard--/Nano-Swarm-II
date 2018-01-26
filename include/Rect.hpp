#ifndef RECT_HPP
# define RECT_HPP

# include "claws/Vect.hpp"

struct Rect
{
  Rect(const claws::Vect<2, float>& i_pos, const claws::Vect<2, float>& i_size, const claws::Vect<4, float>& i_color):
    pos(i_pos),
    size(i_size),
    color(i_color) {}

  claws::Vect<2, float> pos;
  claws::Vect<2, float> size;
  claws::Vect<4, float> color;
};

#endif /* RECT_HPP */

# -*- coding: utf-8 -*-


require_relative 'helper'


class TestView < Test::Unit::TestCase

  def view (*args, &block)
    Reflex::View.new *args, &block
  end

  def style (*args, &block)
    Reflex::Style.new *args, &block
  end

  def shape (*args, &block)
    Reflex::RectShape.new *args, &block
  end

  def selector (*args, &block)
    Reflex::Selector.new *args, &block
  end

  def point  (*args) Rays::Point.new *args end
  def bounds (*args) Rays::Bounds.new *args end

  def test_hidden ()
    v = view
    assert_equal false, v.hidden?
    v.hide
    assert_equal true, v.hidden?
    v.show
    assert_equal false, v.hidden?
  end

  def test_hidden_count ()
    v = view
    v.show
    assert_equal false, v.hidden?
    v.hide
    assert_equal false, v.hidden?
    v.hide
    assert_equal true, v.hidden?
  end

  def test_add_child ()
    assert_raise(ArgumentError) {view.add_child}
    assert_raise(TypeError) {view.add_child nil}
    assert_raise(TypeError) {view.add_child 1}
    v = view
    assert_raise(ArgumentError) {v.add_child v}
    assert_nothing_raised {v.add_child view}
  end

  def test_remove_child ()
    assert_raise(ArgumentError) {view.remove_child}
    assert_raise(TypeError) {view.remove_child nil}
    assert_raise(TypeError) {view.remove_child 1}
    assert_nothing_raised {view.remove_child view}
    v = view
    assert_raise(ArgumentError) {v.remove_child v}
    v.add_child child = view
    assert_nothing_raised {v.remove_child child}
  end

  def test_find_children ()
    v = view
    v.add_child child = view(:name => :Test)
    assert_equal [child], v.find_children(:Test)
  end

  def test_children ()
    me = self
    v = view
    v.add view{set name: '1'}
    v.add view{set name: '2'; add me.view{set name: '2/1'}}
    assert_equal %w[1 2], v.children.map(&:name)
  end

  def test_style ()
    v = view
    s = v.style
    assert_equal 0,   s.margin_left.value
    assert_equal nil, s.margin_left.type
    assert_equal '',  s.margin_left.to_s

    s.margin_left = '100px'
    assert_equal 100,     s.margin_left.value
    assert_equal :px,     s.margin_left.type
    assert_equal '100px', s.margin_left.to_s
  end

  def test_add_remove_style ()
    v, s = view, style(name: :N)
    assert_equal 0,     v.styles.to_a.size

    v.add_style s
    assert_equal 1,     v.styles.to_a.size
    assert_includes     v.find_styles(selector name: :N), s

    v.remove_style s
    assert_equal 0,     v.styles.to_a.size
    assert_not_includes v.find_styles(selector name: :N), s
  end

  def test_shape ()
    s = shape
    assert_equal 0, s.density
    s.density =  1
    assert_equal 1, s.density

    v = view
    assert_not_equal s, v.shape

    v.shape = s
    assert_equal s, v.shape
    assert_equal 1, v.shape.density
    s.density =  2
    assert_equal 2, s.density
    assert_equal 2, v.shape.density

    v.shape =    nil
    assert_equal nil, v.shape
    assert_equal 2, s.density
    assert_equal 0, v.density
    assert_raise(Rucy::NativeError) {v.density = 3}
  end

  def test_add_remove_shape ()
    s = shape name: :S
    s.density =  1
    assert_equal 1, s.density

    v = view
    assert_equal 0,     v.shapes.to_a.size

    v.add_shape s
    assert_equal 1,     v.shapes.to_a.size
    assert_includes     v.find_shapes(selector name: :S), s
    assert_equal 1,     v.find_shapes(selector name: :S).first.density

    v.remove_shape s
    assert_equal 0,     v.shapes.to_a.size
    assert_not_includes v.find_shapes(selector name: :S), s
  end

  def test_name ()
    v = view
    assert_equal nil, v.name
    v.name =     'A'
    assert_equal 'A', v.name
    v.name =     :B
    assert_equal 'B', v.name
    assert_equal 'C', view(:name => 'C').name
  end

  def test_frame ()
    v = view
    b = v.frame.dup
    assert_equal b, v.frame

    v.frame =  1;                 assert_equal [0, 0,    1, 1],     v.frame.to_a
    v.frame =  1.5;               assert_equal [0, 0,    1.5, 1.5], v.frame.to_a
    v.frame = [1];                assert_equal [0, 0,    1, 1],     v.frame.to_a
    v.frame = [1, 2];             assert_equal [0, 0,    1, 2],     v.frame.to_a
    v.frame = [1, 2, 3];          assert_equal [0, 0,    1, 2],     v.frame.to_a
    v.frame = [1, 2, 3];          assert_equal [0, 0, 0, 1, 2, 3],  v.frame.to_a(3)
    v.frame = [1, 2, 3, 4];       assert_equal [1, 2,    3, 4],     v.frame.to_a
    v.frame = [1, 2, 3, 4];       assert_equal [1, 2, 0, 3, 4, 0],  v.frame.to_a(3)
    v.frame = [1, 2, 3, 4, 5, 6]; assert_equal [1, 2,    4, 5],     v.frame.to_a
    v.frame = [1, 2, 3, 4, 5, 6]; assert_equal [1, 2, 3, 4, 5, 6],  v.frame.to_a(3)

    v.frame =  point(1);                        assert_equal [0, 0,    1, 1],    v.frame.to_a
    v.frame = [point(1)];                       assert_equal [0, 0,    1, 1],    v.frame.to_a
    v.frame =  point(1, 2);                     assert_equal [0, 0,    1, 2],    v.frame.to_a
    v.frame = [point(1, 2)];                    assert_equal [0, 0,    1, 2],    v.frame.to_a
    v.frame = [point(1, 2),    point(3, 4)];    assert_equal [1, 2,    3, 4],    v.frame.to_a
    v.frame = [point(1, 2),    point(3, 4)];    assert_equal [1, 2, 0, 3, 4, 0], v.frame.to_a(3)
    v.frame = [point(1, 2, 3), point(4, 5, 6)]; assert_equal [1, 2,    4, 5],    v.frame.to_a
    v.frame = [point(1, 2, 3), point(4, 5, 6)]; assert_equal [1, 2, 3, 4, 5, 6], v.frame.to_a(3)

    v.frame =  bounds(1, 2, 3, 4, 5, 6);  assert_equal [1, 2, 3, 4, 5, 6], v.frame.to_a(3)
    v.frame = [bounds(1, 2, 3, 4, 5, 6)]; assert_equal [1, 2, 3, 4, 5, 6], v.frame.to_a(3)
  end

  def test_parent ()
    parent, child = view, view
    parent.add_child child
    assert_nil parent.parent
    assert_equal parent, child.parent
  end

  def test_density ()
    v = view
    assert_equal 0, v.density
    v.density =  1
    assert_equal 1, v.density
    v.density    2
    assert_equal 2, v.density
  end

  def test_friction ()
    v = view
    assert_equal 0, v.friction
    v.friction = 1
    assert_equal 1, v.friction
    v.friction   2
    assert_equal 2, v.friction
  end

  def test_restitution ()
    v = view
    assert_equal    0, v.restitution
    v.restitution = 1
    assert_equal    1, v.restitution
    v.restitution   2
    assert_equal    2, v.restitution
  end

  def test_sensor ()
    v = view
    assert_equal false, v.sensor?
    v.sensor =   true
    assert_equal true,  v.sensor?
    v.sensor     false
    assert_equal false, v.sensor?
  end

  def test_category ()
    p, v = view, view
    p.add v
    assert_equal [:all], v.category
  end

  def test_collision ()
    p, v = view, view
    p.add v
    #assert_equal [:all], v.collision
  end

end# TestView

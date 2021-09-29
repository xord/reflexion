# -*- coding: utf-8 -*-


require_relative 'helper'


class TestWindow < Test::Unit::TestCase

  def win(*args, &block)
    Reflex::Window.new(*args, &block)
  end

  def point(*args)  Reflex::Point.new(*args) end
  def bounds(*args) Reflex::Bounds.new(*args) end

  def test_show_hide_hidden()
    w = win
    assert_equal true,  w.hidden
    w.show
    assert_equal false, w.hidden
    w.hide
    assert_equal true,  w.hidden
    w.hide
    assert_equal true,  w.hidden
    w.show
    assert_equal true,  w.hidden
    w.show
    assert_equal false, w.hidden
  end

  def test_title()
    w = win
    assert_equal '',  w.title
    w.title = 'A'
    assert_equal 'A', w.title
  end

  def test_frame()
    w = win
    b = w.frame.dup
    assert_equal b, w.frame

    w.frame =  1;                 assert_equal [0, 0,    1, 1],     w.frame.to_a
    w.frame = [1];                assert_equal [0, 0,    1, 1],     w.frame.to_a
    w.frame = [1, 2];             assert_equal [0, 0,    1, 2],     w.frame.to_a
    w.frame = [1, 2, 3];          assert_equal [0, 0,    1, 2],     w.frame.to_a
    w.frame = [1, 2, 3];          assert_equal [0, 0, 0, 1, 2, 0],  w.frame.to_a(3)
    w.frame = [1, 2, 3, 4];       assert_equal [1, 2,    3, 4],     w.frame.to_a
    w.frame = [1, 2, 3, 4];       assert_equal [1, 2, 0, 3, 4, 0],  w.frame.to_a(3)
    w.frame = [1, 2, 3, 4, 5, 6]; assert_equal [1, 2,    4, 5],     w.frame.to_a
    w.frame = [1, 2, 3, 4, 5, 6]; assert_equal [1, 2, 0, 4, 5, 0],  w.frame.to_a(3)

    w.frame =  point(1);                        assert_equal [0, 0,    1, 1],    w.frame.to_a
    w.frame = [point(1)];                       assert_equal [0, 0,    1, 1],    w.frame.to_a
    w.frame =  point(1, 2);                     assert_equal [0, 0,    1, 2],    w.frame.to_a
    w.frame = [point(1, 2)];                    assert_equal [0, 0,    1, 2],    w.frame.to_a
    w.frame = [point(1, 2),    point(3, 4)];    assert_equal [1, 2,    3, 4],    w.frame.to_a
    w.frame = [point(1, 2),    point(3, 4)];    assert_equal [1, 2, 0, 3, 4, 0], w.frame.to_a(3)
    w.frame = [point(1, 2, 3), point(4, 5, 6)]; assert_equal [1, 2,    4, 5],    w.frame.to_a
    w.frame = [point(1, 2, 3), point(4, 5, 6)]; assert_equal [1, 2, 0, 4, 5, 0], w.frame.to_a(3)

    w.frame =  bounds(1, 2, 3, 4, 5, 6);  assert_equal [1, 2, 0, 4, 5, 0], w.frame.to_a(3)
    w.frame = [bounds(1, 2, 3, 4, 5, 6)]; assert_equal [1, 2, 0, 4, 5, 0], w.frame.to_a(3)
  end

  def test_resizable?()
    w = win
    assert_true  w.resizable?

    w.resizable = false
    assert_false w.resizable?

    w.resizable = true
    assert_true  w.resizable?

    w.resizable false
    assert_false w.resizable?

    w.resizable true
    assert_true  w.resizable?
  end

  def test_root()
    w = win
    assert_not_nil       w.root
    assert_nil           w.root.parent
    assert_equal 'ROOT', w.root.name
    assert_equal w,      w.root.window
  end

end# TestWindow

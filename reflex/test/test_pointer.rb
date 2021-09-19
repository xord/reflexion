# -*- coding: utf-8 -*-


require_relative 'helper'


class TestPointer < Test::Unit::TestCase

  TYPE_NONE = Reflex::Pointer::TYPE_NONE
  LEFT      = Reflex::Pointer::MOUSE_LEFT
  RIGHT     = Reflex::Pointer::MOUSE_RIGHT
  MIDDLE    = Reflex::Pointer::MOUSE_MIDDLE
  TOUCH     = Reflex::Pointer::TOUCH
  PEN       = Reflex::Pointer::PEN

  ACTION_NONE = Reflex::Pointer::ACTION_NONE
  DOWN        = Reflex::Pointer::DOWN
  UP          = Reflex::Pointer::UP
  MOVE        = Reflex::Pointer::MOVE
  STAY        = Reflex::Pointer::STAY
  CANCEL      = Reflex::Pointer::CANCEL

  T = true
  F = false

  def pointer(
    type: TYPE_NONE, action: ACTION_NONE, time: 0,
    position: 0, modifiers: 0, click_count: 0, drag: false)

    Reflex::Pointer.new(
      type, action, time, position, modifiers, click_count, drag)
  end

  def test_initialize()
    assert_nothing_raised {pointer}

    p = pointer(
      type: TOUCH, action: DOWN, time: 1,
      position: [2, 3], modifiers: 4, click_count: 5, drag: true)

    assert_equal    [:touch],      p.type
    assert_equal    :down,         p.action
    assert_equal    1,             p.time
    assert_equal    [2, 3],        p.position.to_a
    assert_equal    4,             p.modifiers
    assert_equal    5,             p.click_count
    assert_equal    true,          p.drag?
  end

  def test_type()
    def type(t)
      pointer(type: t).tap do |o|
        def o.test()
          [type, mouse?, left?, right?, middle?, touch?, pen?]
        end
      end
    end

    o = type TYPE_NONE
    assert_equal [[],              F, F, F, F, F, F], o.test

    o = type LEFT
    assert_equal [[:mouse_left],   T, T, F, F, F, F], o.test

    o = type RIGHT
    assert_equal [[:mouse_right],  T, F, T, F, F, F], o.test

    o = type MIDDLE
    assert_equal [[:mouse_middle], T, F, F, T, F, F], o.test

    o = type TOUCH
    assert_equal [[:touch],        F, F, F, F, T, F], o.test

    o = type PEN
    assert_equal [[:pen],          F, F, F, F, F, T], o.test

    o = type LEFT | RIGHT
    actions = [:mouse_left, :mouse_right]
    assert_equal [actions, T, T, T, F, F, F], o.test

    o = type LEFT | RIGHT | MIDDLE
    actions = [:mouse_left, :mouse_right, :mouse_middle]
    assert_equal [actions, T, T, T, T, F, F], o.test

    o = type LEFT | RIGHT | MIDDLE | TOUCH | PEN
    actions = [:mouse_left, :mouse_right, :mouse_middle, :touch, :pen]
    assert_equal [actions, T, T, T, T, T, T], o.test
  end

  def test_action()
    def action(a)
      pointer(action: a).tap do |o|
        def o.test()
          [action, down?, up?, move?, cancel?, stay?]
        end
      end
    end

    o = action ACTION_NONE
    assert_equal [:none,   F, F, F, F, F], o.test

    o = action DOWN
    assert_equal [:down,   T, F, F, F, F], o.test

    o = action UP
    assert_equal [:up,     F, T, F, F, F], o.test

    o = action MOVE
    assert_equal [:move,   F, F, T, F, F], o.test

    o = action CANCEL
    assert_equal [:cancel, F, F, F, T, F], o.test

    o = action STAY
    assert_equal [:stay,   F, F, F, F, T], o.test
  end

  def test_position()
    assert_equal [1, 1], pointer(position: 1     ).position.to_a
    assert_equal [2, 3], pointer(position: [2, 3]).position.to_a
  end

  def test_xy()
    assert_equal 1, pointer(position: [1, 2]).x
    assert_equal 2, pointer(position: [1, 2]).y
  end

  def test_compare()
    assert_equal pointer, pointer

    assert_not_equal pointer, pointer(type:        Reflex::Pointer::PEN)
    assert_not_equal pointer, pointer(action:      Reflex::Pointer::UP)
    assert_not_equal pointer, pointer(time:        1)
    assert_not_equal pointer, pointer(position:    2)
    assert_not_equal pointer, pointer(modifiers:   3)
    assert_not_equal pointer, pointer(click_count: 4)
    assert_not_equal pointer, pointer(drag:        true)
  end

end# TestPointer

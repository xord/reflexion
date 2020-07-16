# -*- coding: utf-8 -*-


require_relative 'helper'


class TestPointerEvent < Test::Unit::TestCase

  E = Reflex::PointerEvent
  T = true
  F = false

  TYPE_NONE = E::TYPE_NONE
  DOWN      = E::TYPE_DOWN
  UP        = E::TYPE_UP
  MOVE      = E::TYPE_MOVE

  POINTER_NONE = E::POINTER_NONE
  LEFT         = E::POINTER_MOUSE_LEFT
  RIGHT        = E::POINTER_MOUSE_RIGHT
  MIDDLE       = E::POINTER_MOUSE_MIDDLE
  TOUCH        = E::POINTER_TOUCH
  PEN          = E::POINTER_PEN

  def event (
    type = TYPE_NONE, pointer_type = POINTER_NONE,
    modifiers = 0, count = 0, drag = false,
    positions: [0])

    Reflex::PointerEvent.new type, pointer_type, modifiers, count, drag, positions
  end

  def test_initialize ()
    assert_nothing_raised       {event positions: 10.times.to_a}
    assert_raise(ArgumentError) {event positions: 11.times.to_a}
    assert_raise(ArgumentError) {event positions: []}
  end

  def test_type ()
    def type (arg)
      event(arg).tap do |o|
        def o.test ()
          [type, down?, up?, move?]
        end
      end
    end

    o = type TYPE_NONE
    assert_equal [:none, F, F, F], o.test

    o = type DOWN
    assert_equal [:down, T, F, F], o.test

    o = type UP
    assert_equal [:up,   F, T, F], o.test

    o = type MOVE
    assert_equal [:move, F, F, T], o.test
  end

  def test_pointer_type ()
    def pointer_type (arg)
      event(TYPE_NONE, arg).tap do |o|
        def o.test ()
          [pointer_type, left?, right?, middle?, touch?, pen?]
        end
      end
    end

    o = pointer_type POINTER_NONE
    assert_equal [[],              F, F, F, F, F], o.test

    o = pointer_type LEFT
    assert_equal [[:mouse_left],   T, F, F, F, F], o.test

    o = pointer_type RIGHT
    assert_equal [[:mouse_right],  F, T, F, F, F], o.test

    o = pointer_type MIDDLE
    assert_equal [[:mouse_middle], F, F, T, F, F], o.test

    o = pointer_type TOUCH
    assert_equal [[:touch],        F, F, F, T, F], o.test

    o = pointer_type PEN
    assert_equal [[:pen],          F, F, F, F, T], o.test

    o = pointer_type LEFT | RIGHT
    types = [:mouse_left, :mouse_right]
    assert_equal [types, T, T, F, F, F], o.test

    o = pointer_type LEFT | RIGHT | MIDDLE
    types = [:mouse_left, :mouse_right, :mouse_middle]
    assert_equal [types, T, T, T, F, F], o.test

    o = pointer_type LEFT | RIGHT | MIDDLE | TOUCH | PEN
    types = [:mouse_left, :mouse_right, :mouse_middle, :touch, :pen]
    assert_equal [types, T, T, T, T, T], o.test
  end

  def test_size ()
    assert_equal 1, event(positions: [0]   ).size
    assert_equal 2, event(positions: [0, 1]).size
  end

  def test_xy ()
    assert_equal 0, event(positions: [[0, 1], [2, 3]]).x
    assert_equal 1, event(positions: [[0, 1], [2, 3]]).y
  end

  def test_position ()
    assert_equal [0, 1], event(positions: [[0, 1], [2, 3]]).position.to_a
  end

  def test_positions ()
    assert_equal [[0, 0]],         event(positions: [0]   ).positions.map(&:to_a)
    assert_equal [[0, 0], [1, 1]], event(positions: [0, 1]).positions.map(&:to_a)

    assert_equal [[0, 1]],         event(positions: [[0, 1]]        ).positions.map(&:to_a)
    assert_equal [[0, 1], [2, 3]], event(positions: [[0, 1], [2, 3]]).positions.map(&:to_a)
  end

  def test_at ()
    assert_equal [0, 1], event(positions: [[0, 1], [2, 3]])[0].to_a
    assert_equal [2, 3], event(positions: [[0, 1], [2, 3]])[1].to_a
    assert_raise(IndexError) {event(positions: [[0, 1], [2, 3]])[-1]}
    assert_raise(IndexError) {event(positions: [[0, 1], [2, 3]])[2]}
  end

end# TestPointerEvent

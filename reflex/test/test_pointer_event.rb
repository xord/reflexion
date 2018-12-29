# -*- coding: utf-8 -*-


require_relative 'helper'


class TestPointerEvent < Test::Unit::TestCase

  T = true
  F = false

  def event (*args)
    Reflex::PointerEvent.new *args
  end

  def type (arg)
    event(arg).tap do |o|
      def o.test ()
        [type, down?, up?, move?]
      end
    end
  end

  def test_type ()
    o = type Reflex::PointerEvent::TYPE_NONE
    assert_equal [:none, F, F, F], o.test

    o = type Reflex::PointerEvent::TYPE_DOWN
    assert_equal [:down, T, F, F], o.test

    o = type Reflex::PointerEvent::TYPE_UP
    assert_equal [:up,   F, T, F], o.test

    o = type Reflex::PointerEvent::TYPE_MOVE
    assert_equal [:move, F, F, T], o.test
  end

  def pointer_type (arg)
    event(Reflex::PointerEvent::TYPE_NONE, arg).tap do |o|
      def o.test ()
        [pointer_type, left?, right?, middle?, touch?, pen?]
      end
    end
  end

  def test_pointer_type ()
    o = pointer_type Reflex::PointerEvent::POINTER_NONE
    assert_equal [[],              F, F, F, F, F], o.test

    o = pointer_type Reflex::PointerEvent::POINTER_MOUSE_LEFT
    assert_equal [[:mouse_left],   T, F, F, F, F], o.test

    o = pointer_type Reflex::PointerEvent::POINTER_MOUSE_RIGHT
    assert_equal [[:mouse_right],  F, T, F, F, F], o.test

    o = pointer_type Reflex::PointerEvent::POINTER_MOUSE_MIDDLE
    assert_equal [[:mouse_middle], F, F, T, F, F], o.test

    o = pointer_type Reflex::PointerEvent::POINTER_TOUCH
    assert_equal [[:touch],        F, F, F, T, F], o.test

    o = pointer_type Reflex::PointerEvent::POINTER_PEN
    assert_equal [[:pen],          F, F, F, F, T], o.test

    o = pointer_type Reflex::PointerEvent::POINTER_MOUSE_LEFT |
      Reflex::PointerEvent::POINTER_MOUSE_RIGHT
    types = [:mouse_left, :mouse_right]
    assert_equal [types, T, T, F, F, F], o.test

    o = pointer_type Reflex::PointerEvent::POINTER_MOUSE_LEFT |
      Reflex::PointerEvent::POINTER_MOUSE_RIGHT |
      Reflex::PointerEvent::POINTER_MOUSE_MIDDLE
    types = [:mouse_left, :mouse_right, :mouse_middle]
    assert_equal [types, T, T, T, F, F], o.test

    o = pointer_type Reflex::PointerEvent::POINTER_MOUSE_LEFT |
      Reflex::PointerEvent::POINTER_MOUSE_RIGHT |
      Reflex::PointerEvent::POINTER_MOUSE_MIDDLE |
      Reflex::PointerEvent::POINTER_TOUCH |
      Reflex::PointerEvent::POINTER_PEN
    types = [:mouse_left, :mouse_right, :mouse_middle, :touch, :pen]
    assert_equal [types, T, T, T, T, T], o.test
  end

end# TestPointerEvent

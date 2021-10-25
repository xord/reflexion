# -*- coding: utf-8 -*-


require_relative 'helper'


class TestFrameEvent < Test::Unit::TestCase

  def event(*args)
    Reflex::FrameEvent.new(*args)
  end

  def bounds(*args)
    Rays::Bounds.new *args
  end

  def test_initialize()
    e = event bounds(0, 1, 2, 3, 4, 5), 6, 7, 8, 9, 10, 11
    assert_equal bounds(0, 1, 2, 3, 4, 5), e.frame
    assert_equal 6,  e.dx
    assert_equal 7,  e.dy
    assert_equal 8,  e.dw
    assert_equal 9,  e.dh
    assert_equal 10, e.angle
    assert_equal 11, e.dangle
  end

end# TestFrameEvent

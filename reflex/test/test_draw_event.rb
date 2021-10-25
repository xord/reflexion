# -*- coding: utf-8 -*-


require_relative 'helper'


class TestDrawEvent < Test::Unit::TestCase

  def event(*args)
    Reflex::DrawEvent.new(*args)
  end

  def bounds(*args)
    Rays::Bounds.new *args
  end

  def test_initialize()
    e = event 1
    assert_nil              e.painter
    assert_equal bounds(0), e.bounds
    assert_equal 1,         e.dt
    assert_equal 0,         e.fps
  end

end# TestDrawEvent

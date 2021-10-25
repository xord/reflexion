# -*- coding: utf-8 -*-


require_relative 'helper'


class TestUpdateEvent < Test::Unit::TestCase

  def event(*args)
    Reflex::UpdateEvent.new(*args)
  end

  def test_initialize()
    e = event 1, 2
    assert_equal 1, e.now
    assert_equal 2, e.dt
  end

end# TestUpdateEvent

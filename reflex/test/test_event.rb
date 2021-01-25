# -*- coding: utf-8 -*-


require_relative 'helper'


class TestEvent < Test::Unit::TestCase

  def event()
    Reflex::Event.new
  end

  def test_block()
    e = event
    assert_equal false, e.block?
    e.block
    assert_equal true, e.block?
  end

end# TestEvent

# -*- coding: utf-8 -*-


require_relative 'helper'


class TestCaptureEvent < Test::Unit::TestCase

  def cap (*args)
    Reflex::CaptureEvent.new *args.map {|arg|
      Reflex::View.capture_flag.symbols2bits *arg
    }
  end

  def test_begin ()
    c = cap [:key, :pointer], []
    assert_equal [:key, :pointer], c.begin
    assert_equal true, c.begin?(:key)
    assert_equal true, c.begin?(:pointer)
    assert_equal true, c.begin?(:all)

    c = cap [:key], []
    assert_equal [:key], c.begin
    assert_equal true,  c.begin?(:key)
    assert_equal false, c.begin?(:pointer)
    assert_equal false, c.begin?(:all)
  end

  def test_end ()
    c = cap [], [:key, :pointer]
    assert_equal [:key, :pointer], c.end
    assert_equal true, c.end?(:key)
    assert_equal true, c.end?(:pointer)
    assert_equal true, c.end?(:all)

    c = cap [], [:key]
    assert_equal [:key], c.end
    assert_equal true,  c.end?(:key)
    assert_equal false, c.end?(:pointer)
    assert_equal false, c.end?(:all)
  end

end# TestCaptureEvent

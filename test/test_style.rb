# -*- coding: utf-8 -*-


require_relative 'helper'


class TestStyle < Test::Unit::TestCase

  def style (*args, &block)
    Reflex::Style.new *args, &block
  end

  def test_initialize ()
    assert_equal nil, style.name
    assert_equal 'A', style(name: :A).name
    assert_equal 'A', style{self.name = :A}.name
  end

  def test_selector ()
    assert_equal 'A', style(name: :A).selector.name
    assert_equal %w[T], style(tag: :T).selector.tags.to_a
  end

  def test_flow ()
    s = style

    assert_equal [:down, :none], s.flow
    s.flow = :up
    assert_equal [:up, :none],   s.flow
    s.flow = [:left, :down]
    assert_equal [:left, :down], s.flow
    s.flow = :none
    assert_equal [:none, :none], s.flow
    assert_raise(ArgumentError) {s.flow = []}
    assert_raise(ArgumentError) {s.flow = [:down, :right, :none]}
  end

  def test_margin_padding ()
    s = style

    assert_equal [0, 0, 0, 0], s.margin.to_a.map(&:value)
    s.margin   = [1]
    assert_equal [1, 1, 1, 1], s.margin.to_a.map(&:value)
    s.margin   = [1, 2]
    assert_equal [1, 2, 1, 2], s.margin.to_a.map(&:value)
    s.margin   = [1, 2, 3]
    assert_equal [1, 2, 3, 2], s.margin.to_a.map(&:value)
    s.margin   = [1, 2, 3, 4]
    assert_equal [1, 2, 3, 4], s.margin.to_a.map(&:value)

    assert_equal [0, 0, 0, 0], s.padding.to_a.map(&:value)
    s.padding   = [1]
    assert_equal [1, 1, 1, 1], s.padding.to_a.map(&:value)
    s.padding   = [1, 2]
    assert_equal [1, 2, 1, 2], s.padding.to_a.map(&:value)
    s.padding   = [1, 2, 3]
    assert_equal [1, 2, 3, 2], s.padding.to_a.map(&:value)
    s.padding   = [1, 2, 3, 4]
    assert_equal [1, 2, 3, 4], s.padding.to_a.map(&:value)
  end

  def test_background_color ()
    assert_equal [0, 0, 0, 1], style.background_color.to_a
  end

  def test_background_image ()
    assert_equal nil, style.background_image
  end

end# TestStyle

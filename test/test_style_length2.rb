# -*- coding: utf-8 -*-


require_relative 'helper'


class TestStyleLength2 < Test::Unit::TestCase

  def len2 (w = 0, h = 0, &block)
    Reflex::StyleLength2.new width: w, height: h, &block
  end

  def test_initialize ()
    assert_equal 0, len2(*%w[])       .width.value
    assert_equal 1, len2(*%w[1px])    .width.value
    assert_equal 2, len2(*%w[1px 2px]).height.value
    assert_equal :px, len2(*%w[1px]).width.unit
    #assert_equal :%,  len2(*%w[1%]).width.unit
  end

  def test_wh ()
    assert_equal '1px', len2(1)   .width.to_s
    assert_equal '2px', len2(1, 2).height.to_s
  end

  def test_to_a ()
    assert_equal [0, 0],    len2.to_a.map(&:value)
    assert_equal [:px] * 2, len2.to_a.map(&:unit)
    assert_equal [1, 2],    len2(1, 2).to_a.map(&:value)
    assert_equal [:px] * 2, len2(1, 2).to_a.map(&:unit)
  end

  def test_get_set_at ()
    l = len2 1, 2

    assert_equal 1, l[0].value
    assert_equal 2, l[1].value

    l[0] = '10px'
    assert_equal 10, l[0].value
    l[1] = '40px'
    assert_equal 40, l[1].value
    l[-1] = '30px'
    assert_equal 30, l[ 1].value
    assert_equal 30, l[-1].value

    assert_raises(IndexError) {l[2] = '50px'}
  end

end# TestStyleLength2

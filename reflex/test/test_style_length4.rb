# -*- coding: utf-8 -*-


require_relative 'helper'


class TestStyleLength4 < Test::Unit::TestCase

  def len4 (l = 0, t = 0, r = 0, b = 0, &block)
    Reflex::StyleLength4.new left: l, top: t, right: r, bottom: b, &block
  end

  def test_initialize ()
    assert_equal 0, len4(*%w[])               .left.value
    assert_equal 1, len4(*%w[1px])            .left.value
    assert_equal 2, len4(*%w[1px 2px])        .top.value
    assert_equal 3, len4(*%w[1px 2px 3px])    .right.value
    assert_equal 4, len4(*%w[1px 2px 3px 4px]).bottom.value
    assert_equal :px, len4(*%w[1px]).left.unit
    #assert_equal :%,  len4(*%w[1%]).left.unit
  end

  def test_ltrb ()
    assert_equal '1px', len4(1)         .left.to_s
    assert_equal '2px', len4(1, 2)      .top.to_s
    assert_equal '3px', len4(1, 2, 3)   .right.to_s
    assert_equal '4px', len4(1, 2, 3, 4).bottom.to_s
  end

  def test_to_a ()
    assert_equal [0, 0, 0, 0], len4.to_a.map(&:value)
    assert_equal [:px] * 4,    len4.to_a.map(&:unit)
    assert_equal [1, 2, 3, 4], len4(1, 2, 3, 4).to_a.map(&:value)
    assert_equal [:px] * 4,    len4(1, 2, 3, 4).to_a.map(&:unit)
  end

  def test_get_set_at ()
    l = len4 1, 2, 3, 4

    assert_equal 1, l[0].value
    assert_equal 2, l[1].value
    assert_equal 3, l[2].value
    assert_equal 4, l[3].value

    l[0] = '10px'
    assert_equal 10, l[0].value
    l[3] = '40px'
    assert_equal 40, l[3].value
    l[-1] = '30px'
    assert_equal 30, l[ 3].value
    assert_equal 30, l[-1].value

    assert_raises(IndexError) {l[4] = '50px'}
  end

end# TestStyleLength4

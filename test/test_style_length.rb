# -*- coding: utf-8 -*-


require_relative 'helper'


class TestStyleLength < Test::Unit::TestCase

  def len (*args, &block)
    Reflex::StyleLength.new *args, &block
  end

  def test_initialize ()
    assert_equal 0,   len.value
    assert_equal nil, len.unit
    assert_equal 100, len(100).value
    assert_equal :px, len(100).unit
    assert_equal 100, len(100, :px).value
    assert_equal :px, len(100, :px).unit
    #assert_equal :%,  len(100, :%).unit
    assert_equal 100, len('100px').value
    assert_equal :px, len('100px').unit
    #assert_equal :%,  len('100%').unit
  end

  def test_to_s ()
    assert_equal '100px',  len(100,   :px).to_s
    #assert_equal '100%',   len(100,   :%).to_s
    #assert_equal '100.5%', len(100.5, :%).to_s
    assert_raises(Rucy::NativeError) {len.to_s}
    assert ((len.to_s rescue $!.message)) =~ /InvalidStateError/
  end

  def test_inspect ()
    assert_nothing_raised {len.inspect}
  end

end# TestStyleLength

# -*- coding: utf-8 -*-


$RAYS_NOAUTOINIT = true
$REFLEX_NOAUTOINIT = true

require_relative 'helper'


class TestReflex < Test::Unit::TestCase

  def test_init()
    assert_raise(Rucy::NativeError) {Reflex.fin!}
    assert Reflex.init!
    assert_raise(Rucy::NativeError) {Reflex.init!}
    assert Reflex.fin!
  end

end# TestReflex

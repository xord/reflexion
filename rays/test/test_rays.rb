# -*- coding: utf-8 -*-


$RAYS_NOAUTOINIT = true

require_relative 'helper'


class TestRays < Test::Unit::TestCase

  def test_init! ()
    assert_raise(Rucy::NativeError) {Rays.fin!}
    assert Rays.init!
    assert_raise(Rucy::NativeError) {Rays.init!}
    assert Rays.fin!
  end

end# TestRays

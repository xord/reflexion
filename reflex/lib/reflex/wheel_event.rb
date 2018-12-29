# -*- coding: utf-8 -*-


require 'reflex/ext'


module Reflex


  class WheelEvent < Event

    def position ()
      Point.new x, y, z
    end

    def delta ()
      Point.new dx, dy, dz
    end

    def inspect ()
      "#<Reflex::WheelEvent dx:#{dx} dy:#{dy} dz:#{dz} x:#{x} y:#{y} z:#{z} mod:#{modifiers}>"
    end

  end# WheelEvent


end# Reflex

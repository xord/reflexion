# -*- coding: utf-8 -*-


require 'reflex/ext'


module Reflex


  class WheelEvent

    def inspect ()
      "#<Reflex::WheelEvent dx:#{dx} dy:#{dy} dz:#{dz} x:#{x} y:#{y} z:#{z} mod:#{modifiers}>"
    end

  end# WheelEvent


end# Reflex

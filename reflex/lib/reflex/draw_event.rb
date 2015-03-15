# -*- coding: utf-8 -*-


require 'reflex/ext'


module Reflex


  class DrawEvent

    def inspect ()
      "#<Reflex::DrawEvent painter:#{painter} bounds:#{bounds} dt:#{dt} fps:#{fps}>"
    end

  end# DrawEvent


end# Reflex

# -*- coding: utf-8 -*-


require 'reflex/view'


module Reflex


  class ShapeView

    def fill= (*args)
      set_fill Color.color *args
      fill
    end

    def stroke= (*args)
      set_stroke Color.color *args
      stroke
    end

  end# ShapeView


end# Reflex

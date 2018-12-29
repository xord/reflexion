# -*- coding: utf-8 -*-


require 'xot/universal_accessor'
require 'reflex/shape'


module Reflex


  class EllipseShape < Shape

    universal_accessor :angle_from, :angle_to, :hole_size, :nsegment

  end# EllipseShape


end# Reflex

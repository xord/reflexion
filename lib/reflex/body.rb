# -*- coding: utf-8 -*-


require 'xot/setter'


module Reflex


  class Body

    include Xot::Setter

    alias velocity= linear_velocity=
    alias velocity  linear_velocity
    alias meter meter2pixel

  end# Body


end# Reflex

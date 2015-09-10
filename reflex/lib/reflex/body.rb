# -*- coding: utf-8 -*-


require 'xot/setter'
require 'xot/universal_accessor'


module Reflex


  class Body

    include Xot::Setter

    extend Xot::UniversalAccessor

    alias meter meter2pixel
    alias static?  static
    alias dynamic? dynamic
    alias velocity= linear_velocity=
    alias velocity  linear_velocity
    alias apply_impulse apply_linear_impulse

    universal_accessor :static, :dynamic, :density, :friction, :restitution,
      :linear_velocity, :angular_velocity, :velocity, :gravity_scale

  end# Body


end# Reflex

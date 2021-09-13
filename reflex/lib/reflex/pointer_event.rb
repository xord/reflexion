# -*- coding: utf-8 -*-


require 'forwardable'
require 'reflex/ext'


module Reflex


  class PointerEvent < Event

    extend Forwardable

    def_delegators :first,
      :type, :mouse?, :touch?, :pen?,
      :mouse_left?, :left?, :mouse_right?, :right?, :mouse_middle?, :middle?,
      :action, :down?, :up?, :move?, :cancel?, :stay?,
      :position, :pos, :x, :y,
      :modifiers, :click_count, :drag?

    def pointers()
      to_enum :each
    end

    def inspect()
      "#<Reflex::PointerEvent type:#{type} act:#{action} x:#{x} y:#{y} mod:#{modifiers} click:#{click_count} drag:#{drag?}>"
    end

    private

    def first()
      self[0]
    end

  end# PointerEvent


end# Reflex

# -*- coding: utf-8 -*-


require 'xot/bit_flag_accessor'
require 'xot/const_symbol_accessor'
require 'reflex/ext'


module Reflex


  class PointerEvent

    alias pos position

    alias get_type         type
    alias get_pointer_type pointer_type

    const_symbol_reader :type, {
      none: TYPE_NONE,
      down: TYPE_DOWN,
      up:   TYPE_UP,
      move: TYPE_MOVE
    }

    bit_flag_reader :pointer_type, {
      none:         POINTER_NONE,
      mouse_left:   POINTER_MOUSE_LEFT,
      mouse_right:  POINTER_MOUSE_RIGHT,
      mouse_middle: POINTER_MOUSE_MIDDLE,
      touch:        POINTER_TOUCH,
      pen:          POINTER_PEN
    }

    def down? ()
      get_type == TYPE_DOWN
    end

    def up? ()
      get_type == TYPE_UP
    end

    def move? ()
      get_type == TYPE_MOVE
    end

    def left? ()
      (get_pointer_type & POINTER_MOUSE_LEFT) != 0
    end

    def right? ()
      (get_pointer_type & POINTER_MOUSE_RIGHT) != 0
    end

    def middle? ()
      (get_pointer_type & POINTER_MOUSE_MIDDLE) != 0
    end

    def touch? ()
      (get_pointer_type & POINTER_TOUCH) != 0
    end

    def pen? ()
      (get_pointer_type & POINTER_PEN) != 0
    end

    def inspect ()
      "#<Reflex::PointerEvent type:#{type}/#{pointer_type} x:#{x} y:#{y} size:#{size} mod:#{modifiers} count:#{count} drag:#{drag?}>"
    end

  end# PointerEvent


end# Reflex

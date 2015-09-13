# -*- coding: utf-8 -*-


require 'xot/const_symbol_accessor'
require 'reflex/ext'


module Reflex


  class KeyEvent

    alias get_type type

    const_symbol_reader :type, {
      none: TYPE_NONE,
      down: TYPE_DOWN,
      up:   TYPE_UP
    }

    def down? ()
      get_type == TYPE_DOWN
    end

    def up? ()
      get_type == TYPE_UP
    end

    def inspect ()
      "#<Reflex::KeyEvent type:#{type} chars:'#{chars}' code:#{code} mod:#{modifiers} repeat:#{repeat} capture?:#{capture?}>"
    end

  end# KeyEvent


end# Reflex

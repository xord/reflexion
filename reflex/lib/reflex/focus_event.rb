# -*- coding: utf-8 -*-


require 'xot/const_symbol_accessor'
require 'reflex/ext'


module Reflex


  class FocusEvent

    alias get_type type

    const_symbol_reader :type, {
      none:  TYPE_NONE,
      focus: TYPE_FOCUS,
      blur:  TYPE_BLUR
    }

    def focus? ()
      get_type == TYPE_FOCUS
    end

    def blur? ()
      get_type == TYPE_BLUR
    end

    def inspect ()
      "#<Reflex::FocusEvent type:#{type} focus:#{focus} last:#{last}>"
    end

  end# FocusEvent


end# Reflex

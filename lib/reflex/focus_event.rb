# -*- coding: utf-8 -*-


require 'reflex/ext'


module Reflex


  class FocusEvent

    def type ()
      TYPE2SYM[get_type] || :none
    end

    def focus? ()
      get_type == TYPE_FOCUS
    end

    def blur? ()
      get_type == TYPE_BLUR
    end

    def inspect ()
      "#<Reflex::FocusEvent type:#{type} focus:#{focus} last:#{last}>"
    end

    private

      TYPE2SYM = {
        TYPE_FOCUS => :focus,
        TYPE_BLUR  => :blur,
      }

  end# FocusEvent


end# Reflex

# -*- coding: utf-8 -*-


require 'reflex/flags'


module Reflex


  class CaptureEvent

    def begin ()
      CAPTURE_FLAG.bits2symbols get_begin
    end

    def begin? (*symbols)
      bits = CAPTURE_FLAG.symbols2bits *symbols
      (bits & get_begin) == bits
    end

    def end ()
      CAPTURE_FLAG.bits2symbols get_end
    end

    def end? (*symbols)
      bits = CAPTURE_FLAG.symbols2bits *symbols
      (bits & get_end) == bits
    end

    def inspect ()
      "#<Reflex::CaptureEvent begin:#{self.begin} end:#{self.end}}>"
    end

  end# CaptureEvent


end# Reflex

# -*- coding: utf-8 -*-


require 'xot/bit_flag'
require 'reflex/ext'


module Reflex


  CAPTURE_FLAG = Xot::BitFlag.new do
    flag :key,     View::CAPTURE_KEY
    flag :pointer, View::CAPTURE_POINTER
    flag :all,     View::CAPTURE_ALL
  end


end# Reflex

# -*- coding: utf-8 -*-


%w[xot beeps rays reflex]
  .map  {|s| File.expand_path "../../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'reflexion/include'


setup do
  flow    :none
  size    600, 400
  gravity 0, 9.8 * meter
  #debug   true
end

draw do
  text "#{event.fps.to_i} FPS", 10, 10
end

pointer do
  if down? || drag?
    window.add [RectShape, EllipseShape].sample.new {
      pos     event.pos
      size    rand 20..40
      fill    event.right? ? :gray : [:red, :green, :blue].sample
      dynamic event.left?
      density 1
      interval(1) {fill rand, rand, rand}
    }
  end
end

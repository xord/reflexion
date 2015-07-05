# -*- coding: utf-8 -*-


%w[xot rays reflex]
  .map  {|s| File.expand_path "../../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'reflexion/include'


setup do
  style.flow = :none
  set size: [600, 400], gravity: Point.new(0, 9.8) * meter #, debug: true
end

draw do
  text "#{event.fps.to_i} FPS", 10, 10
end

pointer do
  if down? || drag?
    window.add [RectShape, EllipseShape].sample.new {
      size  = rand 20..40
      color = event.right? ? :gray : [:red, :green, :blue].sample
      set frame: [*event.pos.to_a, size, size], fill: color
      set dynamic: event.left?, density: 1
    }
  end
end

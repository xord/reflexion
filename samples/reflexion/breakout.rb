# -*- coding: utf-8 -*-


%w[xot rays reflex]
  .map  {|s| File.expand_path "../../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'reflexion/include'


$garbages = []

def add_shape (klass: RectShape, frame: [0, 0, 100, 100], color: :white, type: :static)
  window.add klass.new {
    set frame: frame, fill: color, density: 1, friction: 0, restitution: 1, type => true
  }
end

setup do
  set size: [600, 400], friction: 0
  5.times do |y|
    10.times do |x|
      shape = add_shape frame: [(x + 1) * 50, (y + 1) * 20, 30, 10], color: [:white, :red, :green, :blue, :yellow][y]
      shape.on(:contact) {$garbages << shape}
    end
  end
  $bar   = add_shape frame: [0, 350, 100, 20], color: :blue
  bottom = add_shape frame: [0, window.h - 1, window.w, 1]
  bottom.on(:contact) {|e| $garbages << e.view}
end

pointer do |e|
  $bar.x = e.x - $bar.w / 2
  if e.down?
    ball = add_shape klass: [EllipseShape, RectShape].sample, frame: [e.x, $bar.y - 20, 20, 20], type: :dynamic 
    ball.velocity = Point.new(rand(-1.0..1.0), -1).normal * 500
  end
end

update do
  $garbages.uniq.each {|o| o.parent.remove o}
  $garbages.clear
end

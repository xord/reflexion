# -*- coding: utf-8 -*-


%w[xot rays reflex]
  .map  {|s| File.expand_path "../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'reflex'


Reflex::Application.start name: 'Visuals' do |app|
  Reflex::Window.new title: app.name, frame: [100, 100, 600, 500], flow: :none do |win|
    win.painter.background = 0.8
    10.times do |i|
      win.add Reflex::TextView.new name: 'hello', pos: [i * 5] * 2, data: 'HELLO WORLD!'
    end
  end.show
end

require 'rake/clean'
require 'rake/loaders/makefile'

SRC_DIR = 'src'
DEP_DIR = 'dep'
OBJ_DIR = 'obj'
BIN_DIR = 'bin'

# Binary used for compilation and dependency generation.
CXX = 'g++'
# Binary used for linking.
LD = 'g++'

PACKAGES = %w[opencv glew libglfw glu fontconfig ftgl lua]

# Flags used for compilation.
$cxx_flags = "-c -Wall -std=gnu++0x -ggdb -pipe -D_NYEH_DEBUG -I#{OBJ_DIR} -I#{SRC_DIR}"
# Flags used for dependency generation.
$dep_flags = "-MM -std=gnu++0x -pipe -D_NYEH_DEBUG -MG -MP -I#{SRC_DIR}"
# Flags used for linking.
$ld_flags = "-ggdb -pipe -lboost_thread -lboost_program_options -lboost_regex -lrt -lv4l2"

PRE_SOURCE = File.join SRC_DIR, 'pre.hxx'
PRE_HEADER = File.join OBJ_DIR, 'pre.hxx.gch'
EXECUTABLE = File.join BIN_DIR, 'nyeh.x'

OBJECTS = %w[
  main
  Cam
  HighguiCam
  UvcCam
  HistogramHand
  3DView
  GameUpdater
  Net
  NetCam
  NetGame
  HudRenderer
  HandToModel
  CamRenderer
  Elapsed
  HandFilter
  Calibrate
]

CLEAN.include PRE_HEADER
CLOBBER.include EXECUTABLE

def prerequisites_max_timestamp task
  class << Rake::Task[task]
    def timestamp
      prerequisites.map do |i|
        Rake::Task[i].timestamp
      end.max
    end
  end
end

task :pkg_config do
  pkg = PACKAGES.join ' '
  cxx_pkg_flags = " #{`pkg-config --cflags #{pkg}`.strip}"
  ld_pkg_flags = " #{`pkg-config --libs #{pkg}`.strip}"
  $cxx_flags << cxx_pkg_flags
  $dep_flags << cxx_pkg_flags
  $ld_flags << ld_pkg_flags
end
class << Rake::Task[:pkg_config]
  # File tasks should not be invoked when depending on
  # :pkg_config, except if they have other dependencies.
  def timestamp
    Time.at(0)
  end
end

desc 'Compile the program binary.'
task :compile => EXECUTABLE

namespace :compile do
  desc 'Compile the precompiled prefix header.'
  task :header => PRE_HEADER

  desc 'Compile the object files.'
  multitask :objects
  prerequisites_max_timestamp 'compile:objects'
end

desc 'Generate header dependecy makefiles.'
multitask :depends
prerequisites_max_timestamp :depends

directory BIN_DIR
directory OBJ_DIR
directory DEP_DIR

file EXECUTABLE => ['compile:objects', BIN_DIR, :pkg_config] do |t|
  inputs = Rake::Task['compile:objects'].prerequisites
  sh "#{LD} #{$ld_flags} #{inputs.join(" ")} -o #{t.name}"
end

file PRE_HEADER => [PRE_SOURCE, OBJ_DIR, :pkg_config] do |t|
  sh "#{CXX} #{$cxx_flags} #{t.prerequisites[0]} -o #{t.name}"
end

OBJECTS.each do |obj|
  source = File.join SRC_DIR, "#{obj}.cxx"
  dep = File.join DEP_DIR, "#{obj}.mf"
  output = File.join OBJ_DIR, "#{obj}.o"

  file dep => [source, DEP_DIR, :pkg_config] do |t|
    sh "#{CXX} #{$dep_flags} -MT #{output} #{t.prerequisites[0]} -MF #{t.name}"
  end

  file output => [source, PRE_HEADER, OBJ_DIR, :pkg_config] do |t|
    sh "#{CXX} #{$cxx_flags} -include pre.hxx #{t.prerequisites[0]} -o #{t.name}"
  end

  multitask 'compile:objects' => output
  multitask :depends => dep
  CLEAN.include dep.to_s, output.to_s
  import dep
end

task :default => :compile

# EventMolecularDynamics
A program implementing event driven molecular dynamics for hard smooth spheres

A more detailed description is on the blog: http://compphys.go.ro/event-driven-molecular-dynamics/

### Tools

The project compiles on Windows with Visual Studio 2015.

### The Code

Especially for the code intended to present some concepts rather than be a production code, I agree with Donald Knuth:

> The real problem is that programmers have spent far too much time worrying about efficiency in the wrong places and at the wrong times; premature optimization is the root of all evil (or at least most of it) in programming. 

I did not focus on optimization. I tried first to make the code work correctly (although I'm not 100% sure it works all right, I did not test it enough). I consider that the code should first express intent, then one should work on optimization.

At first I simply called the `Simulation::Advance` from the main thread and it worked quite all right for plenty of particles. The code that does computations should reside in another thread to avoid locking the main thread, so after that I implemented the `MolecularDynamicsThread`. It uses `Advance` to generate results that are put in a queue accessed from the main thread. It's a quite standard producer/consumer code but it still can lock the main thread if the worker thread cannot keep up with the requests, so don't simulate many particles with a lot of speed. I didn't think much about it, I just wanted to move out the computation from the main thread. What the main thread gets from the worker thread are 'snapshots' of the computation, after each collision. The main thread calculates the actual particle positions using the position, particle time (each particle has its own time) and velocity. Since particles travel in straight lines between collisions, there is no need to move those computations in a separate thread, they are fast enough to not pose problems.

For the same reasons, I used a `set::multiset` for the events priority queue, although it's probably implemented with a tree. I did not care about performance at that point. Later, after the first commit on <a href="https://github.com/aromanro/EventMolecularDynamics" target="blank_">GitHub</a>, I replaced the multiset with a heap. Since the std `priority_queue` exposes `const` iterators, I implemented one myself using std algorithms for a heap. With this change the performance increased a bit. One might do better by looking into heap implementations from boost, but I didn't consider it necessary for the purpose of the project. The commit has a 'WARNING!' in the comment, you might want to get the code with the multiset if you notice bugs in the one with the heap. I turned removing the events from the queue into setting them to 'no event' event type instead. Maybe not very elegant, but it works. Those events are removed when they reach to the top of the heap.

###### OpenGL code

I've got the OpenGL code from the <a href="http://compphys.go.ro/newtonian-gravity/" target="blank_"> Newtonian Gravity project</a> and I customized it to fit this project. First I dropped some classes that are not used, like those for shadow cube map, skybox and textures. I did some copy/paste into the view class for the OpenGL code but there are quite a bit of changes in there. There is also a `MolecularDynamicsGLProgram` class derived from `OpenGL::Program` which has quite different shaders compared with the Solar System project. I also changed the `OpenGL::Sphere` class to allow a `useTexture` parameter at creation. For this project no texture is used, so it is set to false. There is no need of passing info about texture coordinates if it's not going to be used. I also added a `DrawInstanced` method and there is one of the most important change in the OpenGL code compared with the Solar System implementation. In the view you'll notice that there are three `OpenGL::VertexBufferObject`, one for color, one for scale and one for position. The scale is set at `Setup` (for OpenGL) time. In there the color is also set, but that can change later during running. The only thing that changes each frame is the position. The code draws all the spheres by a single call to `DrawInstanced` after setting the position data all at once in the vertex buffer. I already gave some tutorial links in the Newtonian Mechanics post, I'll only point to instancing ones now: <a href="http://www.learnopengl.com/#!Advanced-OpenGL/Instancing" target="blank_">here</a> and <a href="http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/" target="blank_">here</a>. It's not the purpose of this blog to describe in detail OpenGL code so I won't describe the code more. I'll mention that I tested it with more than one directional light and it works. Currently it's a single light only but you can add more in `CMolecularDynamicsView::SetupShaders`.

###### Libraries

As for the Solar System project, the program uses mfc, already included with Visual Studio, the obvious OpenGL library and <a href="http://glm.g-truc.net/0.9.7/index.html" target="_blank">glm</a> and <a href="http://glew.sourceforge.net/" target="_blank">glew</a>. 

###### Classes

I won't describe here the classes from the `OpenGL` namespace, I'll just mention that they are quite thin wrappers around OpenGL API, they typically contain an `ID`. Please visit the <a href="http://compphys.go.ro/newtonian-gravity/" target="blank_"> Newtonian Gravity post</a> for more details.

Here it is a short description of the classes, first the ones generated by the App Wizard:

* `CMolecularDynamicsApp` - the application class. There is not much change in there, just added the contained options and `InitInstance` is changed to load them and also the registry key is changed to `Free App`.
* `CMolecularDynamicsDoc` - the document class. It contains the data. Since it's a SDI application, it is reused (that is, it is the same object after you choose 'New'). There is quite a bit of additions/changes there. It contains the worker thread, the results queue, a mutex that protects the access to the results queue, the 'simulation time' and so on. Some of them are atomic for multithreading access although maybe not all that are actually need to be. I did several changes during moving the computation in the worker thread and I probably left some atomic variables that do not need to be.
* `CMolecularDynamicsView` - the view class. It take care of displaying. Contains quite a bit of OpenGL code, it deals with key presses and the mouse for camera movements. Among other OpenGL objects it contains the camera, too.
* `CMainFrame` - the main frame class. I added there some menu/toolbar handling. It also forwards some mouse and keyboard presses to the view in case it gets them.
* `CAboutDlg` - what the name says.

Now, some classes I 'stole' from my other open source projects:

* `CEmbeddedSlider` and `CMFCToolBarSlider` - they implement the slider from the toolbar, used to adjust the simulation speed.
* `CNumberEdit` - the edit control for editing double/float values.
* `ComputationThread` - the base class for the worker thread.
* `Vector3D` - what the name says. First used in the Solar System project. I already anticipated there that I will be using this in other projects.

Classes dealing with options and the UI for them:

* `Options` - the class the contains the options and allows saving them into registry and loading them from registry.
* `COptionsPropertySheet` - the property sheet. It's as generated by the class wizard except that it loads and sets the icon. I already used it in other projects here.
* `SimulationPropertyPage`, `DisplayPropertyPage`, `CameraPropertyPage` - property pages that are on the property sheet. They are quite standard mfc stuff, I won't say much about them.

The classes that contain the implementation (apart from the document and the view, you should also check those to understand how it works):

* `ComputationResult` - it's what the worker thread passes to the main thread. It contains just a list of particles and the next collision time.
* `MolecularDynamicsGLProgram` - the OpenGL program. It's derived from `OpenGL::Program` and it deals with shaders, lights and so on.
* `Particle` - implements a particle. Contains the particle mass, radius, position and velocity and a 'particle time'. Has some methods that allows finding out the particle position at a particular time (if it suffers no collision until then), the wall collision time and the collision time with other particle.
* `Event` - this implements the events that are in the events queue. The event has a type (currently it can be 'wall collision', 'particles collision' or 'no event'), a time of occurrence and it also contains the particles involved. If only a particle is involved, it is set in particle1, the other should be -1. 
* `Simulation` - as the name says, it is the simulation class. It contains a vector of particles that holds all simulated particles and the events queue, currently implemented with a heap (using a vector container). It has several helper methods, the main one that advances simulation from one collision event to the next one is `Advance`.
* `MolecularDynamicsThread` - the worker thread. It's a typical producer/consumer implementation but I feel it's far from being optimal. My main goal was to have the simulation functional, I didn't focus on performance, so here it is, something that works. It advances the simulation and it puts the results in the document's results queue (if not filled up).
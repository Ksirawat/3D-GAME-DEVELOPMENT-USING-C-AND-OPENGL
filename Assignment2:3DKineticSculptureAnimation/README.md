## ## Description 

My kinetic sculpture is a hierarchical system designed to resemble a small solar system, consisting of a central "Sun," an orbiting "Planet," and a "Moon" that orbits the planet.

* **The Sun:** A large, central cube that rotates slowly around its own Y-axis. It acts as the parent object for the entire system.
* **The Planet:** A medium-sized cube that orbits the Sun. Its movement is a combination of its own rotation and its revolution around the Sun.
* **The Moon:** A small cube that orbits the Planet. Its final position is dependent on both the Planet's and the Sun's transformations, clearly demonstrating the parent-child hierarchy.

This animation is achieved by calculating and multiplying model matrices for each object in the render loop, driven by the application's elapsed time.

<img width="799" height="600" alt="image" src="https://github.com/user-attachments/assets/0fa9eab7-1676-4387-9fbc-b95d9f3e9562" />


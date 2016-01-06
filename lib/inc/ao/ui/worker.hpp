#include <chrono>
#include <future>

#include <Eigen/Dense>

#include "ao/gl/core.hpp"
#include "ao/render/heightmap.hpp"

struct Task;
class Accelerator;

/*
 *  A worker contains all of the data needed for a running render task
 */
struct Worker
{
    /*
     *  Constructs a CPU worker from the given Evaluator and a task
     *  (higher task divisors produce lower-resolution workers)
     *
     *  depth and norm are target textures in which results are stored
     */
    Worker(Evaluator* eval, const Task& task, GLFWwindow* context,
           GLuint depth, GLuint norm);

    /*
     *  Constructs a GPU worker from the given Accelerator and a task
     *  (higher task divisors produce lower-resolution workers)
     *
     *  depth and norm are target textures in which results are stored
     */
    Worker(Accelerator* accel, const Task& task, GLFWwindow* context,
           GLuint depth, GLuint norm);

    /*
     *  Constructs a CPU + GPU worker from the given Accelerator and a task
     *  (higher task divisors produce lower-resolution workers)
     *
     *  depth and norm are target textures in which results are stored
     */
    Worker(Evaluator* eval, Accelerator* accel, const Task& task,
           GLFWwindow* context, GLuint depth, GLuint norm);

    /*
     *  On destruction, join the thread
     */
    ~Worker();

    /*
     *  Returns true if the worker is running, false otherwise
     */
    bool running() const;

    /*
     *  Attempts to halt the task by setting the abort flag
     */
    void halt();

    /*
     *  Polls the worker, loading data into the given textures if complete
     *
     *  Returns RUNNING if the worker is still running, DONE if the worker
     *  is done, and ABORTED if the worker was aborted but is complete.
     */
    enum State { RUNNING, DONE, ABORTED };
    State poll();

    /*  Region that is being analyzed  */
    Region region;

    std::promise<bool> promise;
    std::future<bool> future;
    std::atomic<bool> abort;
    std::thread thread;

protected:
    /*
     *  Private constructor that populates the
     *  region, future, and abort fields
     */
    Worker(const Task& t);

    /*
     *  Record the starting time in start_time
     */
    void start();

    /*
     *  Record the elapsed time in elapsed and call glfwPostEmptyEvent
     */
    void end();

    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::duration<double> elapsed;
};
/**
 * ============================================================================
 * GRAPHICS SYSTEMS PROGRAMMING STUDY SANDBOX (C++20)
 * ============================================================================
 * * This file is designed as a starter project to tackle the critical mechanics 
 * of graphics and game engine programming. It specifically targets:
 * * 1. CONSTEXPR MATH: Pre-calculating vectors/layout indices at compile-time.
 * 2. MEMORY ALIGNMENT: Matching structs to hardware alignments (e.g., std140).
 * 3. SAFE POLYMORPHISM: Handling base-pointer cleanups with virtual destructors.
 * 4. MOVE-ONLY RAII: Designing resource wrappers for OpenGL/Vulkan handles.
 * * ============================================================================
 */

#include <iostream>
#include <memory>
#include <type_traits>
#include <cstddef>

// ============================================================================
// TOPIC 1 & 2: CONSTEXPR MATH, LAYOUTS & ALIGNAS (Addressing GPU Layouts)
// ============================================================================

/**
 * @brief A lightweight, hardware-aligned 3D Vector.
 * * Graphics APIs like Vulkan, OpenGL, and DirectX expect vector data in specific alignments.
 * By using 'constexpr', we can perform operations (like dot products or length evaluations)
 * entirely at compile-time when using constant expressions.
 */
struct alignas(16) Vec3 {
    float x{ 0.0f };
    float y{ 0.0f };
    float z{ 0.0f };
    float w{ 1.0f }; // 4th coordinate (homogenous coordinates) for alignment padding

    // A constexpr constructor allowing compile-time object instantiation
    constexpr Vec3(float xVal, float yVal, float zVal, float wVal = 1.0f)
        : x(xVal), y(yVal), z(zVal), w(wVal) {}

    // Constexpr dot product: Calculated entirely at compile-time if inputs are compile-time constants
    constexpr float dot(const Vec3& other) const {
        return (x * other.x) + (y * other.y) + (z * other.z);
    }
};

/**
 * @brief Simple Point Light conforming to GPU alignment layout rules (std140 standard).
 */
struct alignas(16) PointLight {
    Vec3 position;   // 16 bytes (aligned)
    float color[3];  // 12 bytes
    float intensity; // 4 bytes (together with color completes 16-byte alignment boundary)
};

// Compile-Time Verification checks using standard library type traits.
// These static assertions run entirely during compiling; they generate zero runtime code!
static_assert(alignof(Vec3) == 16, "Vec3 is not 16-byte aligned! Check hardware properties.");
static_assert(std::is_standard_layout_v<PointLight>, "PointLight is not a standard layout! Unsafe for GPU copying.");
static_assert(sizeof(PointLight) == 32, "PointLight size is unexpected. Padding rules must ensure exactly 32 bytes.");

void demonstrateCompileTimeMathAndLayouts() {
    std::cout << "\n--- DEMONSTRATION 1: CONSTEXPR MATH & COMPILER ALIGNMENT ---\n";

    // Because these vectors are constant expressions, dot product is solved at compile-time!
    constexpr Vec3 lightDirection{ 1.0f, 0.0f, 0.0f };
    constexpr Vec3 surfaceNormal{ 0.707f, 0.707f, 0.0f };
    constexpr float diffuseFactor = lightDirection.dot(surfaceNormal);

    // This outputs the compile-time precalculated value directly
    std::cout << "Compile-time Diffuse Factor calculation: " << diffuseFactor << "\n";
    std::cout << "Alignment of Vec3: " << alignof(Vec3) << " bytes\n";
    std::cout << "Size of PointLight Struct: " << sizeof(PointLight) << " bytes (aligned to standard-layout requirements)\n";
}


// ============================================================================
// TOPIC 3: POLYMORPHIC DESTRUCTION (Addressing Quiz Gaps)
// ============================================================================

/**
 * @brief Base Class representing a physical resource allocated on the GPU.
 */
class GPUResource {
protected:
    unsigned int m_ResourceID{ 0 };

public:
    GPUResource(unsigned int id) : m_ResourceID(id) {
        std::cout << "[GPUResource] Resource ID " << m_ResourceID << " Allocated.\n";
    }

    /**
     * @brief VIRTUAL DESTRUCTOR (CRITICAL!)
     * * In C++, if you delete a derived class object through a pointer to its base class,
     * the base class destructor MUST be virtual. If it's not virtual, only the base class
     * destructor is executed, resulting in leaked derived member resources (like GPU allocations).
     */
    virtual ~GPUResource() {
        std::cout << "[GPUResource] Base Resource ID " << m_ResourceID << " Cleaned Up.\n";
    }

    virtual void bind() const = 0; // Pure virtual interface
};

/**
 * @brief Derived Class representing a modern graphics pipeline buffer.
 */
class VertexBuffer : public GPUResource {
private:
    float* m_LocalDataBuffer{ nullptr }; // Represents CPU side memory cached for updates
    size_t m_BufferSize{ 0 };

public:
    VertexBuffer(unsigned int id, size_t size) : GPUResource(id), m_BufferSize(size) {
        // Simulating allocating memory on the heap (or mapping memory)
        m_LocalDataBuffer = new float[m_BufferSize];
        std::cout << "[VertexBuffer] Derived Buffer allocated with size " << m_BufferSize * sizeof(float) << " bytes on Heap.\n";
    }

    // Safely called because of virtual destructor hierarchy!
    ~VertexBuffer() override {
        delete[] m_LocalDataBuffer;
        std::cout << "[VertexBuffer] Derived local float buffer safely freed from CPU memory heap.\n";
    }

    void bind() const override {
        std::cout << "[VertexBuffer] Binding vertex array buffer " << m_ResourceID << " to pipeline.\n";
    }
};

void demonstratePolymorphicDestruction() {
    std::cout << "\n--- DEMONSTRATION 2: SAFE POLYMORPHIC DESTRUCTION ---\n";
    
    // Create a VertexBuffer but store it under a Base pointer
    GPUResource* myBuffer = new VertexBuffer(105, 1024);

    myBuffer->bind();

    // Trigger destruction.
    // If GPUResource's destructor was NOT virtual, only [GPUResource] destructor would run,
    // and 'm_LocalDataBuffer' in VertexBuffer would be permanently leaked!
    // Try removing 'virtual' from ~GPUResource() to observe the catastrophic resource leak.
    delete myBuffer; 
}


// ============================================================================
// TOPIC 4: RAII & MOVE-ONLY SEMANTICS (Essential for OpenGL/Vulkan Handles)
// ============================================================================

/**
 * @brief Safe Move-only Wrapper for a GPU Shader Object.
 * * Shader handles cannot be copied, because copies lead to multiple instances attempting
 * to call 'glDeleteShader' on the same driver handle when exiting scopes (Double-Free Error).
 */
class ShaderProgram {
private:
    unsigned int m_ProgramID{ 0 };

public:
    explicit ShaderProgram(unsigned int id) : m_ProgramID(id) {
        std::cout << "[Shader] Shader program " << m_ProgramID << " generated on GPU.\n";
    }

    // 1. Destructor handles clean up safely (RAII paradigm)
    ~ShaderProgram() {
        if (m_ProgramID != 0) {
            std::cout << "[Shader] Shader program " << m_ProgramID << " explicitly deleted from GPU driver storage.\n";
            m_ProgramID = 0;
        }
    }

    // 2. Prevent Copying entirely (Delete copy constructor and assignment operator)
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    // 3. Implement Move Constructor (Transfer resource ownership safely)
    ShaderProgram(ShaderProgram&& other) noexcept : m_ProgramID(other.m_ProgramID) {
        other.m_ProgramID = 0; // Nullify source so its destructor does not delete GPU resource
        std::cout << "[Shader] Resource transferred via Move Constructor. Handle is now " << m_ProgramID << ".\n";
    }

    // 4. Implement Move Assignment Operator
    ShaderProgram& operator=(ShaderProgram&& other) noexcept {
        if (this != &other) {
            // Free current resource before accepting new one
            if (m_ProgramID != 0) {
                std::cout << "[Shader] Destroying existing handle " << m_ProgramID << " during move assignment.\n";
                m_ProgramID = 0;
            }
            m_ProgramID = other.m_ProgramID;
            other.m_ProgramID = 0; // Nullify source
            std::cout << "[Shader] Resource transferred via Move Assignment. Handle is now " << m_ProgramID << ".\n";
        }
        return *this;
    }

    unsigned int getID() const { return m_ProgramID; }
};

void demonstrateRAIIMoveOnly() {
    std::cout << "\n--- DEMONSTRATION 3: MOVE-ONLY RAII RESOURCE MANAGEMENT ---\n";

    // 1. Instantiation
    ShaderProgram mainShader(402);

    // 2. This would cause a compiling error since copy constructor is deleted:
    // ShaderProgram badCopy = mainShader; 

    // 3. Moving ownership explicitly using std::move (casts mainShader to an rvalue reference)
    std::cout << "Creating new Shader via move transfer...\n";
    ShaderProgram activeShader = std::move(mainShader);

    std::cout << "Old Shader Handle (should be empty/0): " << mainShader.getID() << "\n";
    std::cout << "New Active Shader Handle: " << activeShader.getID() << "\n";
}


// ============================================================================
// MAIN RUNNER
// ============================================================================
int main() {
    std::cout << "=========================================================\n";
    std::cout << "        Starting C++ Graphics Engine Systems Sandbox\n";
    std::cout << "=========================================================\n";

    demonstrateCompileTimeMathAndLayouts();
    demonstratePolymorphicDestruction();
    demonstrateRAIIMoveOnly();

    std::cout << "\n=========================================================\n";
    std::cout << "        Graphics Sandbox Executed Successfully!\n";
    std::cout << "=========================================================\n";
    return 0;
}
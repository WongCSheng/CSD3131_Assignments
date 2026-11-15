import cv2
import numpy as np


def compute_gradient_energy(image: np.ndarray) -> np.ndarray:
    """
    Computes the gradient energy map of an input image.

    Pipeline (this is exactly what the report describes):
      1. Convert to grayscale and scale to float32 in [0, 1].
      2. Light 3x3 Gaussian blur (sigma = 0.5) to suppress pixel noise so that
         noise is not mistaken for real edges.
      3. Sobel gradients Gx and Gy using OpenCV's cv2.Sobel (ksize=3).
         OpenCV applies the Sobel kernel in its separable form internally
         (a 1D smoothing pass [1, 2, 1] and a 1D derivative pass [-1, 0, 1]).
         We do NOT implement our own separable filtering or tiling.
      4. Energy = |Gx| + |Gy| (L1 norm of the gradient).

    Parameters:
        image (np.ndarray): Input image (BGR or Grayscale)

    Returns:
        np.ndarray: 2D float32 energy map representing pixel disruption measures.
    """
    # Convert to grayscale if the image is in color
    if len(image.shape) == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    else:
        gray = image

    # Normalize pixel values to float32 range [0, 1] for precision
    float_img = gray.astype(np.float32) / 255.0

    # Noise reduction before differentiation (see report: Implementation Notes)
    blurred = cv2.GaussianBlur(float_img, (3, 3), sigmaX=0.5)

    # Gradients along X and Y using OpenCV's optimized Sobel implementation
    grad_x = cv2.Sobel(blurred, cv2.CV_32F, 1, 0, ksize=3)
    grad_y = cv2.Sobel(blurred, cv2.CV_32F, 0, 1, ksize=3)

    # Energy map defined as the L1 norm (absolute gradient magnitude)
    energy_map = np.abs(grad_x) + np.abs(grad_y)

    return energy_map


# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

def test_output_shape_and_dtype():
    img = np.random.randint(0, 256, (40, 60, 3), dtype=np.uint8)
    energy = compute_gradient_energy(img)
    assert energy.shape == (40, 60)
    assert energy.dtype == np.float32
    assert np.all(energy >= 0), "Energy is an absolute value, so it can never be negative"


def test_constant_image_has_zero_energy():
    """An image with one flat colour has no gradients, so energy must be 0 everywhere."""
    for value in (0, 77, 255):
        gray = np.full((50, 50), value, dtype=np.uint8)
        color = np.full((50, 50, 3), value, dtype=np.uint8)
        assert np.allclose(compute_gradient_energy(gray), 0.0, atol=1e-5)
        assert np.allclose(compute_gradient_energy(color), 0.0, atol=1e-5)


def test_vertical_edge_is_localized():
    """Left half black, right half white: energy only near the edge columns."""
    img = np.zeros((20, 20), dtype=np.uint8)
    img[:, 10:] = 255
    energy = compute_gradient_energy(img)
    assert energy[:, 9:11].min() > 0.5, "Edge columns should have high energy"
    assert np.allclose(energy[:, :7], 0.0, atol=1e-5), "Flat left region should be 0"
    assert np.allclose(energy[:, 13:], 0.0, atol=1e-5), "Flat right region should be 0"


def test_horizontal_edge_is_localized():
    """Top half black, bottom half white: energy only near the edge rows."""
    img = np.zeros((20, 20), dtype=np.uint8)
    img[10:, :] = 255
    energy = compute_gradient_energy(img)
    assert energy[9:11, :].min() > 0.5
    assert np.allclose(energy[:7, :], 0.0, atol=1e-5)
    assert np.allclose(energy[13:, :], 0.0, atol=1e-5)


def test_linear_ramp_has_constant_interior_gradient():
    """Intensity increasing by a fixed step per column gives a constant Gx in the interior."""
    step = 2  # grey levels per column
    row = (np.arange(30) * step).astype(np.uint8)
    img = np.tile(row, (30, 1))
    energy = compute_gradient_energy(img)
    # Sobel Gx of a ramp = 8 * step (kernel weights 1+2+1=4, spanning 2 columns) / 255
    expected = 8 * step / 255.0
    interior = energy[2:-2, 2:-2]
    assert np.allclose(interior, expected, atol=1e-4)


def test_matches_explicit_separable_filtering():
    """cv2.Sobel should equal the separable form [1,2,1] x [-1,0,1] that the report describes."""
    rng = np.random.default_rng(0)
    blurred = cv2.GaussianBlur(rng.random((40, 40)).astype(np.float32), (3, 3), sigmaX=0.5)
    gx_sobel = cv2.Sobel(blurred, cv2.CV_32F, 1, 0, ksize=3)
    gx_sep = cv2.sepFilter2D(
        blurred, cv2.CV_32F,
        kernelX=np.array([-1, 0, 1], dtype=np.float32),   # derivative along x
        kernelY=np.array([1, 2, 1], dtype=np.float32),    # smoothing along y
    )
    assert np.allclose(gx_sobel, gx_sep, atol=1e-5)


def run_tests():
    tests = [
        test_output_shape_and_dtype,
        test_constant_image_has_zero_energy,
        test_vertical_edge_is_localized,
        test_horizontal_edge_is_localized,
        test_linear_ramp_has_constant_interior_gradient,
        test_matches_explicit_separable_filtering,
    ]
    for t in tests:
        t()
        print(f"PASS  {t.__name__}")
    print(f"\nAll {len(tests)} tests passed.")


if __name__ == "__main__":
    run_tests()
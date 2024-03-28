export module LibraryLinkUtilities.TimeSeries;

import System.Base;
import System.MDArray;

import LibraryLinkUtilities.Base;
import LibraryLinkUtilities.MArgumentQueue;

using namespace std;
using namespace experimental;

namespace LLU {
    /// Represents a constant view of a regular time series.
    /// @tparam T The type of values.
    export template<typename T>
    struct TimeSeriesView {
        double IntervalSeconds; ///< The interval between two values in seconds.
        span<const T> Values; ///< A list of values.

        /// Returns the length of the path.
        /// @return The length of the path.
        [[nodiscard]] int PathLength() const noexcept {
            return static_cast<int>(Values.size());
        }

        /// Returns the duration of the path in seconds.
        /// @return The duration of the path in seconds.
        [[nodiscard]] double DurationSeconds() const noexcept {
            return PathLength() * IntervalSeconds;
        }
    };

    /// Represents a constant view of a collection of regular time series.
    /// @tparam T The type of values.
    export template<typename T>
    struct TemporalDataView {
        double IntervalSeconds; ///< The interval between two values in seconds.
        mdspan<const T, dims<2>> Values; ///< A 2D array of values.

        /// Returns the number of paths in the collection.
        /// @return The number of paths in the collection.
        [[nodiscard]] int PathCount() const noexcept {
            return Values.extent(0);
        }

        /// Returns the length of each path.
        /// @return The length of each path.
        [[nodiscard]] int PathLength() const noexcept {
            return Values.extent(1);
        }

        /// Returns the duration of each path in seconds.
        /// @return The duration of each path in seconds.
        [[nodiscard]] double DurationSeconds() const noexcept {
            return PathLength() * IntervalSeconds;
        }

        /// Returns the i-th path in the collection.
        /// @param index The index of the path.
        /// @return The i-th path in the collection.
        [[nodiscard]] TimeSeriesView<T> operator[](int index) const {
            return {IntervalSeconds, span(&Values(index, 0), PathLength())};
        }
    };
}

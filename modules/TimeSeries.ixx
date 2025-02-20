export module LibraryLinkUtilities.TimeSeries;

import System.Base;
import System.Math;
import System.MDArray;

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
        /// @returns The length of the path.
        [[nodiscard]] int PathLength() const noexcept {
            return static_cast<int>(Values.size());
        }

        /// Returns the duration of the path in seconds.
        /// @returns The duration of the path in seconds.
        [[nodiscard]] double DurationSeconds() const noexcept {
            return PathLength() * IntervalSeconds;
        }

        /// Returns a subset of the time series within the specified window.
        /// @param offsetSeconds The offset of the window in seconds.
        /// @param durationSeconds The duration of the window in seconds.
        /// @returns A subset of the time series within the specified window.
        [[nodiscard]] TimeSeriesView Window(double offsetSeconds, double durationSeconds) const {
            const auto offset = Math::Ceiling(offsetSeconds / IntervalSeconds);
            const auto count = Math::Ceiling((offsetSeconds + durationSeconds) / IntervalSeconds) - offset;
            return {IntervalSeconds, Values.subspan(offset, count)};
        }
    };

    /// Represents a constant view of a collection of regular time series.
    /// @tparam T The type of values.
    export template<typename T>
    struct TemporalDataView {
        double IntervalSeconds; ///< The interval between two values in seconds.
        mdspan<const T, dims<2>> Values; ///< A 2D array of values.

        /// Returns the number of paths in the collection.
        /// @returns The number of paths in the collection.
        [[nodiscard]] int PathCount() const noexcept {
            return static_cast<int>(Values.extent(0));
        }

        /// Returns the length of each path.
        /// @returns The length of each path.
        [[nodiscard]] int PathLength() const noexcept {
            return static_cast<int>(Values.extent(1));
        }

        /// Returns the duration of each path in seconds.
        /// @returns The duration of each path in seconds.
        [[nodiscard]] double DurationSeconds() const noexcept {
            return PathLength() * IntervalSeconds;
        }

        /// Returns the path at the specified index.
        /// @param index The index of the path.
        /// @returns The path at the specified index.
        [[nodiscard]] TimeSeriesView<T> operator[](int index) const {
            return {IntervalSeconds, span(&Values[index, 0], PathLength())};
        }

        /// Returns a subset of the temporal data within the specified window.
        /// @param offsetSeconds The offset of the window in seconds.
        /// @param durationSeconds The duration of the window in seconds.
        /// @returns A subset of the temporal data within the specified time.
        [[nodiscard]] TemporalDataView Window(double offsetSeconds, double durationSeconds) const {
            const auto offset = Math::Ceiling(offsetSeconds / IntervalSeconds);
            const auto count = Math::Ceiling((offsetSeconds + durationSeconds) / IntervalSeconds) - offset;
            return {IntervalSeconds, submdspan(Values, full_extent, pair(offset, offset + count))};
        }
    };
}
